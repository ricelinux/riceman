#pragma once

#include "progress_bar.hpp"

#include <string>

#include <git2.h>

class GitRepository
{
    public:

    GitRepository(const std::string &path, const std::string &remote);
    ~GitRepository();
    void clone();
    void checkout_commit(std::string &hash);
    void pull();
    void checkout_default();

    bool cloned;

    private:

    int handle_libgit_error(int error);
    static int fetch_progress(const git_indexer_progress *stats, void *payload);
    static void cred_acquire(git_credential **out, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload);

    git_repository *repo;
    const std::string path;
    const std::string remote_uri;
};