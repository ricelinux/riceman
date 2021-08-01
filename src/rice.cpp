#include "rice.hpp"

Rice::Rice(std::string name, std::string id, std::string description, std::string version, std::string window_manager, std::vector<Dependency> dependencies, bool installed)
: name{name}, id{id}, description{description}, version{version}, window_manager{window_manager}, dependencies{dependencies}, installed{installed}, toml_path{fmt::format("{}/{}.toml", LOCAL_CONFIG_DIR, id)}, git_path{fmt::format("{}/{}", LOCAL_RICES_DIR, id)}
{}

static int sideband_progress(const char *str, int len, void *payload)
{
	(void)payload; /* unused */

	printf("remote: %.*s", len, str);
	fflush(stdout);
	return 0;
}

static int fetch_progress(const git_indexer_progress *stats, void *payload)
{
	progress_data *pd = (progress_data*)payload;
	pd->fetch_progress = *stats;
	std::cout << "Fetch progress: " << pd->fetch_progress.received_objects << "/" << pd->fetch_progress.total_objects << std::endl;
    std::cout << "Delta progress: " << pd->fetch_progress.indexed_deltas << "/" << pd->fetch_progress.total_deltas << std::endl;
    return 0;
}
static void checkout_progress(const char *path, size_t cur, size_t tot, void *payload)
{
	progress_data *pd = (progress_data*)payload;
	pd->completed_steps = cur;
	pd->total_steps = tot;
	pd->path = path;
	std::cout << cur << "/" << tot << std::endl;
}

void Rice::handle_libgit_error(int &error)
{
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
    int error;
    git_object *commit;
    char commit_hash[40];
    const git_oid *commit_oid;
    
    error = git_revparse_single(&commit, repo, "HEAD");
    if (error < 0) handle_libgit_error(error);
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
        checkout_opts.progress_cb = checkout_progress;
        checkout_opts.progress_payload = &pd;
        clone_opts.checkout_opts = checkout_opts;
        clone_opts.fetch_opts.callbacks.sideband_progress = sideband_progress;
        clone_opts.fetch_opts.callbacks.transfer_progress = &fetch_progress;
        clone_opts.fetch_opts.callbacks.credentials = (git_credential_acquire_cb)Rice::cred_acquire;
        clone_opts.fetch_opts.callbacks.payload = &pd;

        int error = git_clone(&repo, git_repo_uri.c_str(), git_path.c_str(), &clone_opts);
        if (error < 0) handle_libgit_error(error);
    }

    progress_bar->done();
    delete progress_bar;

}