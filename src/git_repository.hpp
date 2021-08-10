#pragma once

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
    const std::string get_head_hash();

    bool cloned;

    private:

    void handle_libgit_error(int error);
    static void cred_acquire(git_credential **out, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload);

    git_repository *repo;
    const std::string path;
    const std::string remote_uri;
};