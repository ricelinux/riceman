#include "rice.hpp"
#include "utils.hpp"
#include "constants.hpp"
#include "git_repository.hpp"

#include <fmt/format.h>
#include <cpr/cpr.h>

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

Rice::Rice(std::string name, std::string description, std::string new_version, std::string window_manager, std::string hash, DependencyVec new_dependencies)
: name{name}, description{description}, version{new_version}, window_manager{window_manager},
    new_dependencies{new_dependencies}, toml_path{fmt::format("{}/{}.toml", LOCAL_CONFIG_DIR, name)}, 
    toml_tmp_path{fmt::format("{}.tmp", toml_path)}, git_path{fmt::format("{}/{}", LOCAL_RICES_DIR, name)}, 
    install_state{NOT_INSTALLED}, hash{hash}
{
    /* If .toml and git are installed */
    if (fs::exists(toml_path) && fs::is_regular_file(toml_path)) install_state = install_state | TOML_INSTALLED;
    if (fs::exists(git_path) && fs::is_directory(git_path)) install_state = install_state | GIT_INSTALLED;

    if ((install_state & TOML_INSTALLED) == 0) return;

    /* If rice is up-to-date */
    auto rice_config = cpptoml::parse_file(toml_path);

    desktop_path = get_desktop_path(rice_config);

    std::string old_version = rice_config->get_qualified_as<std::string>("theme.version")
        .value_or("");
    std::vector<std::string> pacman_deps = rice_config->get_qualified_array_of<std::string>("packages.pacman")
        .value_or<std::vector<std::string>>({});
    std::vector<std::string> aur_deps = rice_config->get_qualified_array_of<std::string>("packages.aur")
        .value_or<std::vector<std::string>>({});

    if (new_version.length() == 0) throw std::runtime_error{fmt::format("theme version not specified in '{}' config", name)};

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

    if (fs::exists(desktop_path) && fs::is_regular_file(desktop_path)) {
        install_state |= DESKTOP_INSTALLED;
        if (new_version == old_version && (install_state & GIT_INSTALLED) != 0) install_state |= UP_TO_DATE;
    }
}

void Rice::download_toml(ProgressBar *pb)
{
    using namespace std::placeholders;
    cpr::Response r = cpr::Get(cpr::Url{fmt::format("{}/{}.toml", REMOTE_RICES_URI, name)}, cpr::ProgressCallback{std::bind(&ProgressBar::progress_callback_download, pb, _1, _2, _3, _4)});

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
    wm_path = rice_config->get_qualified_as<std::string>("window-manager.executable").value_or("");
    wm_params = fmt::format(rice_config->get_qualified_as<std::string>("window-manager.params").value_or(""), fmt::arg("RICE_DIR", git_path));

    if (wm_path.length() == 0) throw std::runtime_error{fmt::format("window manager path not specified in '{}' config", name)};
    if (wm_params.length() == 0) throw std::runtime_error{fmt::format("window manager params not specified in '{}' config", name)};
    if (git_repo_uri.length() == 0) throw std::runtime_error{fmt::format("git repository uri not specified in '{}' config", name)};
    if (git_commit_hash.length() == 0) throw std::runtime_error{fmt::format("git commit hash not specified in '{}' config", name)};
}

void Rice::install_git(ProgressBar *pb, int &rice_index, int rice_count)
{
    GitRepository git_repo{git_path, git_repo_uri};

    if (!git_repo.cloned) git_repo.clone();
    else if ((install_state & Rice::UP_TO_DATE) == 0) {
        git_repo.checkout_default();
        git_repo.pull();
    }
    git_repo.checkout_commit(git_commit_hash);
}

void Rice::install_desktop()
{
    /* Write desktop file */
    std::string file_content = fmt::format(
        "[Desktop Entry]\nName={0}\nComment={1}\nExec=env RICE_DIR={2} {3} {4}\nType=Application\nPath={2}\n",
        name, description, git_path, wm_path, wm_params);

    Utils::write_file_content(desktop_path.empty() ? get_desktop_path(cpptoml::parse_file(toml_path)) : desktop_path, file_content);
}

Rice Rice::from_string(std::string &db_line) {
    std::stringstream line_stream{db_line};
    std::string rice_data[6];
    DependencyVec deps;
    int valuei = 0;
    
    /* Parse comma separated list into array */
    for(std::string value; std::getline(line_stream, value, ','); ) {
        rice_data[valuei] = value;
        ++valuei;
    }
    if (valuei != 6) throw std::runtime_error{"malformatted database string"};

    /* Parse dependencies */
    std::stringstream deps_stream{rice_data[4]};
    for(std::string dependency; std::getline(deps_stream, dependency, ';'); ) {
        int slash_loc = dependency.find("/");
        bool aur;
        std::string type = dependency.substr(0, slash_loc);
        if (type.compare("pacman") == 0) aur = false;
        else if (type.compare("aur") == 0) aur = true;
        else throw std::runtime_error{"incorrect dependency type in database string"};

        deps.push_back({
            aur,
            dependency.substr(slash_loc + 1, dependency.length())
        });
    }
    return Rice(rice_data[0], rice_data[1], rice_data[2], rice_data[3], rice_data[5], deps);
}

std::string Rice::get_desktop_path(std::shared_ptr<cpptoml::table> rice_config)
{
    std::string path = rice_config->get_qualified_as<std::string>("window-manager.display-server")
        .value_or("");
    
    if (path == "xorg") path = fmt::format("{}/{}.desktop", XSESSIONS_PATH, name);
    else if (path == "wayland") path = fmt::format("{}/{}.desktop", WSESSIONS_PATH, name);
    else throw std::runtime_error{fmt::format("invalid display server specified in '{}' config", name)};

    return path;
}