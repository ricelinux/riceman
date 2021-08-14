#include "git_repository.hpp"

#include <filesystem>
#include <iostream>

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

void GitRepository::fetch(git_remote **remote)
{
    git_fetch_options fetch_options = GIT_FETCH_OPTIONS_INIT;

    handle_libgit_error(git_remote_lookup(remote, repo, "origin"));
    handle_libgit_error(git_remote_fetch(*remote, NULL, &fetch_options, NULL));
}

bool GitRepository::merge_default(git_remote **remote)
{
    /* Perform `git merge origin/master` */
    git_oid branch_oid;
    git_merge_analysis_t analysis;
    git_merge_preference_t preference;
    git_annotated_commit *heads[ 1 ];

    git_repository_fetchhead_foreach(repo, [](
        const char *ref_name,
        const char *remote_url,
        const git_oid *oid,
        unsigned int is_merge,
        void *payload) -> int {
            if (is_merge) *(git_oid*)payload = *oid;
            else *(git_oid*)payload = {};
            return 0;
    }, &branch_oid);

    if (git_oid_is_zero(&branch_oid)) return false;
    if (git_annotated_commit_lookup( &heads[ 0 ], repo, &branch_oid)) return false;
    
    handle_libgit_error(git_merge_analysis(&analysis, &preference, repo, (const git_annotated_commit **)heads, 1));

    if (analysis & GIT_MERGE_ANALYSIS_UP_TO_DATE) {
        git_annotated_commit_free(heads[0]);
        git_repository_state_cleanup(repo);
        git_remote_free(*remote);
        return true;
    } else if (analysis & GIT_MERGE_ANALYSIS_FASTFORWARD) {
        git_buf ref_name;
        git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;
        git_proxy_options proxy_opts = GIT_PROXY_OPTIONS_INIT;

        handle_libgit_error(git_remote_connect(*remote, GIT_DIRECTION_FETCH, &callbacks, &proxy_opts, NULL));
        if (!git_remote_connected(*remote)) throw std::runtime_error{fmt::format("failed to connect to '{}'", remote_uri)};
        handle_libgit_error(git_remote_default_branch(&ref_name, *remote));
        handle_libgit_error(git_remote_disconnect(*remote));
        
        git_reference *ref;
        git_reference *newref;

        if (git_reference_lookup(&ref, repo, ref_name.ptr) == 0)
            git_reference_set_target(&newref, ref, &branch_oid, "pull: Fast-forward");
        
        git_reset_from_annotated(repo, heads[0], GIT_RESET_HARD, NULL);

        git_reference_free(ref);
        git_reference_free(newref);
        git_buf_free(&ref_name);
    }

    git_annotated_commit_free(heads[0]);
    git_repository_state_cleanup(repo);
    git_remote_free(*remote);
    return true;
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