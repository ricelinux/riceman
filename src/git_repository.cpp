#include "git_repository.hpp"

#include <filesystem>

#include <fmt/format.h>

namespace fs = std::filesystem;

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

void GitRepository::pull()
{
    git_remote *remote;
    git_fetch_options fetch_options = GIT_FETCH_OPTIONS_INIT;

    handle_libgit_error(git_remote_lookup(&remote, repo, "origin"));
    handle_libgit_error(git_remote_fetch(remote, NULL, &fetch_options, NULL));
}

const std::string GitRepository::get_head_hash()
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

void GitRepository::handle_libgit_error(int error)
{
    if (error >= 0) return;
    const git_error *err = git_error_last();
    if (err) throw std::runtime_error{fmt::format("{} (error code {})", err->message, err->klass)};
    else throw std::runtime_error{fmt::format("unknown libgit2 error occurred (error code {})", error)};
    git_error_clear();
}

void GitRepository::cred_acquire(git_credential **out, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload)
{
    return throw std::runtime_error{fmt::format("'{}' requires additional authentication", url)};
}