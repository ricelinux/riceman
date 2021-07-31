#include "rice.hpp"
#include "progressbar.hpp"
#include "constants.hpp"
#include "utils.hpp"

#include <fmt/format.h>
#include <cpr/cpr.h>
#include <git2.h>
#include <cpptoml.h>

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
	std::cout << pd->completed_steps << "/" << pd->total_steps << std::endl;
	return 0;
}
static void checkout_progress(const char *path, size_t cur, size_t tot, void *payload)
{
	progress_data *pd = (progress_data*)payload;
	pd->completed_steps = cur;
	pd->total_steps = tot;
	pd->path = path;
	std::cout << pd->completed_steps << "/" << pd->total_steps << std::endl;
}

void Rice::cred_acquire(git_credential **out, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload) {
    return throw std::runtime_error{fmt::format("'{}' requires additional authentication", url)};
}

void Rice::install()
{
    git_libgit2_init();

    /* Download rice's toml */
    using namespace std::placeholders;
    ProgressBar clone_progress_bar{"cloning rice repo", 0.4};
    ProgressBar progress_bar{fmt::format("downloading rice config", name, version), 0.4};
    cpr::Response r = cpr::Get(cpr::Url{fmt::format("{}/{}.toml", REMOTE_RICES_URI, id)}, cpr::ProgressCallback{std::bind(&ProgressBar::progress_callback_download, &progress_bar, _1, _2, _3, _4)});
    progress_bar.done();

    if (!Utils::write_file_content(toml_path, r.text)) throw std::runtime_error{fmt::format("unable to write to '{}'", toml_path)};

    /* Parse TOML */
    auto rice_config = cpptoml::parse_file(toml_path);
    
    git_repo_uri = rice_config->get_qualified_as<std::string>("git.repo").value_or("");
    git_commit_hash = rice_config->get_qualified_as<std::string>("git.commit").value_or("");

    if (git_repo_uri.length() == 0) throw std::runtime_error{fmt::format("git repository uri not specified in '{}' config", name)};
    if (git_commit_hash.length() == 0) throw std::runtime_error{fmt::format("git commit hash not specified in '{}' config", name)};

    /* Clone git repo */
    progress_data pd = {{0}};
    git_repository *repo = nullptr;
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

    int clone_error = git_clone(&repo, git_repo_uri.c_str(), git_path.c_str(), &clone_opts);
    if (clone_error != 0) {
        const git_error *err = git_error_last();
		if (err) throw std::runtime_error{fmt::format("{} (error code {})", err->message, err->klass)};
		else throw std::runtime_error{fmt::format("error cloning git repository", clone_error)};
    }
}