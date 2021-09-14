#pragma once

#include "package_manager.hpp"
#include "progress_bar.hpp"

#include <string>

class DatabaseRice
{
    public:

    DatabaseRice(std::string name, std::string description, std::string new_version, std::string window_manager, std::string hash, DependencyVec dependencies);
    void download_toml(std::string path, ProgressBar *pb);
    static DatabaseRice from_string(std::string &database_line);

    short install_state;

    const std::string name;
    const std::string description;
    const std::string version;
    const std::string window_manager;
    const std::string hash;
    const std::string toml_path;
    const std::string git_path;

    DependencyVec dependencies;

    enum {
        NOT_INSTALLED     = 0b000,
        TOML_INSTALLED    = 0b001,
        GIT_INSTALLED     = 0b010,
        UP_TO_DATE        = 0b100,
    };
};