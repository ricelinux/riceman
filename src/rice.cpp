#include "rice.hpp"
#include "utils.hpp"
#include "constants.hpp"
#include "progressbar.hpp"

#include <fmt/format.h>
#include <cpr/cpr.h>

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

Rice::Rice(std::string name, std::string id, std::string description, std::string new_version, std::string window_manager, std::string hash, DependencyVec new_dependencies)
: name{name}, id{id}, description{description}, version{new_version}, window_manager{window_manager},
    new_dependencies{new_dependencies}, toml_path{fmt::format("{}/{}.toml", LOCAL_CONFIG_DIR, id)}, 
    toml_tmp_path{fmt::format("{}.tmp", toml_path)}, git_path{fmt::format("{}/{}", LOCAL_RICES_DIR, id)}, 
    install_state{NOT_INSTALLED}, hash{hash}
{
    /* If .toml and git are installed */
    if (fs::exists(toml_path) && fs::is_regular_file(toml_path)) install_state = install_state | TOML_INSTALLED;
    if (fs::exists(git_path) && fs::is_directory(git_path)) install_state = install_state | GIT_INSTALLED;

    /* If rice is up-to-date */
    if (install_state != BOTH_INSTALLED) return;

    auto rice_config = cpptoml::parse_file(toml_path);
    
    std::string old_version = rice_config->get_qualified_as<std::string>("theme.version")
        .value_or("");
    std::vector<std::string> pacman_deps = rice_config->get_qualified_array_of<std::string>("packages.pacman")
        .value_or<std::vector<std::string>>({});
    std::vector<std::string> aur_deps = rice_config->get_qualified_array_of<std::string>("packages.aur")
        .value_or<std::vector<std::string>>({});

    if (new_version.length() == 0) throw std::runtime_error{fmt::format("theme version not specified in '{}' config", name)};
    // NOTE: No need to check deps because there might be a case of a rice without dependencies

    if (new_version == old_version) install_state |= UP_TO_DATE;

    for (std::string &dep_name : pacman_deps) {
        old_dependencies.push_back({
            false,
            dep_name
        });
    }

    for (std::string &dep_name : aur_deps) {
        old_dependencies.push_back({
            true,
            dep_name
        });
    }
}

void Rice::handle_libgit_error(int error)
{
    if (error >= 0) return;
    const git_error *err = git_error_last();
    if (err) throw std::runtime_error{fmt::format("{} (error code {})", err->message, err->klass)};
    else throw std::runtime_error{fmt::format("unknown libgit2 error occurred (error code {})", error)};  
    git_error_clear();
}

const bool Rice::repo_exists(const std::string &path, git_repository **repo)
{
    if (!fs::exists(path) || !fs::is_directory(path)) return false;

    int error = git_repository_open_ext(repo, path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);

    return error >= 0;
}

const std::string Rice::get_head_hash(git_repository *repo)
{
    git_object *commit;
    char commit_hash[40];
    const git_oid *commit_oid;
    
    handle_libgit_error(git_revparse_single(&commit, repo, "HEAD"));

    if (git_object_type(commit) == GIT_OBJECT_COMMIT) {
        commit_oid = git_commit_id((git_commit*)commit);
        git_oid_tostr(commit_hash, 40, commit_oid);
        git_object_free(commit);

        return commit_hash;
    }
    
    /* This should never happen */
    throw std::runtime_error{"incorrect git object type for 'HEAD'"};
}

void Rice::cred_acquire(git_credential **out, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload) {
    return throw std::runtime_error{fmt::format("'{}' requires additional authentication", url)};
}

void Rice::download_toml(ProgressBar *pb)
{
    using namespace std::placeholders;
    cpr::Response r = cpr::Get(cpr::Url{fmt::format("{}/{}.toml", REMOTE_RICES_URI, id)}, cpr::ProgressCallback{std::bind(&ProgressBar::progress_callback_download, pb, _1, _2, _3, _4)});

    /* Handle potential errors */
    if (r.error) throw std::runtime_error{fmt::format("{} (error code {})", r.error.message, r.error.code)};

    if (!Utils::write_file_content(toml_tmp_path, r.text)) throw std::runtime_error{fmt::format("unable to write to '{}'", toml_tmp_path)};
}

bool Rice::verify_toml()
{
    return Utils::hash_file(toml_tmp_path) == hash;
}

void Rice::parse_toml()
{
    /* Parse TOML (called cpptoml::parse_file in constructor and here because file is re-downloaded here) */
    auto rice_config = cpptoml::parse_file(toml_tmp_path);
    
    git_repo_uri = rice_config->get_qualified_as<std::string>("git.repo").value_or("");
    git_commit_hash = rice_config->get_qualified_as<std::string>("git.commit").value_or("");
    display_server = rice_config->get_qualified_as<std::string>("window-manager.display-server").value_or("");
    wm_path = rice_config->get_qualified_as<std::string>("window-manager.executable").value_or("");
    wm_params = fmt::format(rice_config->get_qualified_as<std::string>("window-manager.params").value_or(""), git_path);

    if (display_server.length() == 0) throw std::runtime_error{fmt::format("display server not specified in '{}' config", name)};
    if (wm_path.length() == 0) throw std::runtime_error{fmt::format("window manager path not specified in '{}' config", name)};
    if (wm_params.length() == 0) throw std::runtime_error{fmt::format("window manager params not specified in '{}' config", name)};
    if (git_repo_uri.length() == 0) throw std::runtime_error{fmt::format("git repository uri not specified in '{}' config", name)};
    if (git_commit_hash.length() == 0) throw std::runtime_error{fmt::format("git commit hash not specified in '{}' config", name)};
}

void Rice::install_git()
{
    /* Check if git repo exists, verify commit hash */
    git_repository *repo = NULL;
    if (!repo_exists(git_path, &repo)) {
        /* Clone git repo */
        git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
        git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;

        checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
        clone_opts.checkout_opts = checkout_opts;
        clone_opts.fetch_opts.callbacks.credentials = (git_credential_acquire_cb)Rice::cred_acquire;

        handle_libgit_error(git_clone(&repo, git_repo_uri.c_str(), git_path.c_str(), &clone_opts));
    }

    /* Checkout repo with hash */
    git_commit *commit;
    git_oid commit_oid;
    git_checkout_options final_checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
    final_checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
    
    handle_libgit_error(git_oid_fromstr(&commit_oid, git_commit_hash.c_str()));
    handle_libgit_error(git_commit_lookup(&commit, repo, &commit_oid));
    handle_libgit_error(git_checkout_tree(repo, (const git_object *)commit, &final_checkout_opts));

    /* Update HEAD */
    handle_libgit_error(git_repository_set_head_detached(repo, &commit_oid));
    git_commit_free(commit);
}

void Rice::install_desktop()
{
    /* Get correct session directory */
    std::string session_path;
    if (display_server == "xorg") {
        session_path = XSESSIONS_PATH;
    } else if (display_server == "wayland") {
        session_path = WSESSIONS_PATH;
    } else throw std::runtime_error{fmt::format("invalid display server specified in '{}' config", name)};
    
    /* Write desktop file */
    std::string file_content = fmt::format(
        "[Desktop Entry]\nName={0}\nComment={1}\nExec=env RICE_DIR={2} {3} {4}\nType=Application\nPath={2}\n",
        name, description, git_path, wm_path, wm_params);

    Utils::write_file_content(fmt::format("{}/{}.desktop", session_path, id), file_content);
}