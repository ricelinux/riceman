#pragma once

#include <string>

#include <git2.h>

class GitRepository
{
    public:

    GitRepository(std::string &path, std::string &remote);
    void clone();
    void checkout_commit(std::string &hash);
    void pull();

    private:

    git_repository *repo;
    std::string path;
    std::string remote;
    bool cloned;
};