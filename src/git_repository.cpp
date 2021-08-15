#include "git_repository.hpp"
#include "utils.hpp"

#include <filesystem>
#include <iostream>

#include <fmt/format.h>

namespace fs = std::filesystem;

#define GIT "/usr/bin/git"

typedef struct fetch_progress_payload {
    ProgressBar *progress_bar;
    int *rice_index;
    int *rice_count;
} fetch_progress_payload;

typedef struct fetchhead_foreach_payload {
    const git_oid *branch_oid;
    const char **branch_name;
} fetchhead_foreach_payload;

GitRepository::GitRepository(const std::string &path, const std::string &remote_uri)
    : path{path}, remote_uri{remote_uri}
{
    if (fs::exists(path) && fs::is_directory(path)) {
        int error = git_repository_open_ext(&repo, path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
        cloned = error >= 0;
    } else {
        cloned = false;
    }
}

GitRepository::~GitRepository()
{
    git_repository_free(repo);
}

void GitRepository::clone()
{
    git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
    clone_opts.checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
    clone_opts.checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
    clone_opts.fetch_opts.callbacks.credentials = (git_credential_acquire_cb)GitRepository::cred_acquire;
    
    handle_libgit_error(git_clone(&repo, remote_uri.c_str(), path.c_str(), &clone_opts));
}

void GitRepository::checkout_commit(std::string &hash)
{
    git_commit *commit;
    git_oid commit_oid;
    git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
    checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;

    handle_libgit_error(git_oid_fromstr(&commit_oid, hash.c_str()));
    handle_libgit_error(git_commit_lookup(&commit, repo, &commit_oid));
    handle_libgit_error(git_checkout_tree(repo, (const git_object *)commit, &checkout_opts));

    /* Update head */
    handle_libgit_error(git_repository_set_head_detached(repo, &commit_oid));
    git_commit_free(commit);
}

// todo: Figure out how to implement `git pull` and `git checkout -` into pure libgit
// note: My brain is hemorrhaging

void GitRepository::pull()
{
    char * const args[] = {GIT, "-C", (char *const)path.data(), "pull", NULL};
    Utils::exec(args);
}

void GitRepository::checkout_default()
{
    git_reference *origin_head;
    git_object *treeish;
    git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
    checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;

    std::string origin_default_branch;

    /* Get default branch */
    handle_libgit_error(git_reference_lookup(&origin_head, repo, "refs/remotes/origin/HEAD"));
    origin_default_branch = git_reference_symbolic_target(origin_head);
    origin_default_branch = origin_default_branch.substr(20, origin_default_branch.length());

    /* Checkout default branch */
    handle_libgit_error(git_revparse_single(&treeish, repo, origin_default_branch.data()));
    handle_libgit_error(git_checkout_tree(repo, treeish, &checkout_opts));

    /* Set HEAD to default branch */
    handle_libgit_error(git_repository_set_head(repo, fmt::format("refs/heads/{}", origin_default_branch).data()));
}


int GitRepository::fetch_progress(const git_indexer_progress *stats, void *payload)
{
    fetch_progress_payload *pd = (fetch_progress_payload*)payload;
    double index = *(pd->rice_index);
    double count = *(pd->rice_count);
    double percent = (stats->indexed_deltas + stats->indexed_objects) / (stats->total_deltas + stats->total_objects);
    
    pd->progress_bar->update("", ((index + percent) / count));

    return 0;
}

int GitRepository::handle_libgit_error(int error)
{
    if (error >= 0) return error;
    const git_error *err = git_error_last();
    if (err) throw std::runtime_error{fmt::format("{} (error code {})", err->message, err->klass)};
    else throw std::runtime_error{fmt::format("unknown libgit2 error occurred (error code {})", error)};
}

void GitRepository::cred_acquire(git_credential **out, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload)
{
    return throw std::runtime_error{fmt::format("'{}' requires additional authentication", url)};
}