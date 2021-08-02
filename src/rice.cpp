#include "rice.hpp"

// Needs to be defined here because of static functions
ProgressBar *progress_bar;

Rice::Rice(std::string name, std::string id, std::string description, std::string version, std::string window_manager, std::vector<Dependency> dependencies, bool installed)
: name{name}, id{id}, description{description}, version{version}, window_manager{window_manager}, dependencies{dependencies}, installed{installed}, toml_path{fmt::format("{}/{}.toml", LOCAL_CONFIG_DIR, id)}, git_path{fmt::format("{}/{}", LOCAL_RICES_DIR, id)}
{}

int Rice::fetch_progress(const git_indexer_progress *stats, void *payload)
{   
    progress_bar->update("", ((double)stats->indexed_deltas+(double)stats->received_objects) / ((double)stats->total_deltas+(double)stats->total_objects));
    return 0;
}
void Rice::checkout_progress(const char *path, size_t cur, size_t tot, void *payload)
{
    if (cur == 0) {
        progress_bar->done();
        delete progress_bar;

        progress_bar = new ProgressBar{"checking out commit", 0.4};
    }
    progress_bar->update("", cur / (tot + 1));
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

void Rice::install()
{
    /* Download rice's toml */
    using namespace std::placeholders;
    progress_bar = new ProgressBar{"downloading rice config", 0.4};
    cpr::Response r = cpr::Get(cpr::Url{fmt::format("{}/{}.toml", REMOTE_RICES_URI, id)}, cpr::ProgressCallback{std::bind(&ProgressBar::progress_callback_download, progress_bar, _1, _2, _3, _4)});
    progress_bar->done();

    delete progress_bar;

    if (!Utils::write_file_content(toml_path, r.text)) throw std::runtime_error{fmt::format("unable to write to '{}'", toml_path)};

    /* Parse TOML */
    auto rice_config = cpptoml::parse_file(toml_path);
    
    git_repo_uri = rice_config->get_qualified_as<std::string>("git.repo").value_or("");
    git_commit_hash = rice_config->get_qualified_as<std::string>("git.commit").value_or("");

    if (git_repo_uri.length() == 0) throw std::runtime_error{fmt::format("git repository uri not specified in '{}' config", name)};
    if (git_commit_hash.length() == 0) throw std::runtime_error{fmt::format("git commit hash not specified in '{}' config", name)};

    /* Check if git repo exists, verify commit hash */
    progress_bar = new ProgressBar{"cloning repository", 0.4};

    git_repository *repo = NULL;
    if (!repo_exists(git_path, &repo)) {
        /* Clone git repo */
        progress_data pd = {{0}};
        git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
        git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
        
        checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
        checkout_opts.progress_cb = Rice::checkout_progress;
        checkout_opts.progress_payload = &pd;
        clone_opts.checkout_opts = checkout_opts;
        clone_opts.fetch_opts.callbacks.transfer_progress = Rice::fetch_progress;
        clone_opts.fetch_opts.callbacks.credentials = (git_credential_acquire_cb)Rice::cred_acquire;
        clone_opts.fetch_opts.callbacks.payload = &pd;

        handle_libgit_error(git_clone(&repo, git_repo_uri.c_str(), git_path.c_str(), &clone_opts));

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

    progress_bar->done();
    delete progress_bar;
    

}