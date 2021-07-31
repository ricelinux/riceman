#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include <git2.h>

#define CREATE_DIRECTORY(path) \
    if (fs::exists(path) && !fs::is_directory(path)) fs::remove(path); \
    if (!fs::exists(path)) { \
        fs::create_directory(path); \
        fs::permissions(path, fs::perms::owner_all | fs::perms::group_exec | fs::perms::group_read | fs::perms::others_read | fs::perms::others_exec); \
    }

namespace fs = std::filesystem;

typedef struct Dependency {
    bool aur;
    std::string name;
} Dependency;

class Rice
{
    public:

    Rice(std::string name, std::string id, std::string description, std::string version, std::string window_manager, std::vector<Dependency> dependencies, bool installed);

    void install();

    bool installed;
    const std::string name;
    const std::string id;
    const std::string description;
    const std::string version;
    const std::string window_manager;
    const std::string file_path;
    std::vector<Dependency> dependencies; 

    private:

    void download_toml();
    void clone_progress(const git_transfer_progress *stats, void *payload);

};