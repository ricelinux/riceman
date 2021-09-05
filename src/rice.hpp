#pragma once

#include "progressbar.hpp"
#include "package_manager.hpp"

#include <string>
#include <vector>
#include <chrono>

#include <git2.h>
#include <cpptoml.h>

class Rice
{
    public:

    Rice(std::string name, 
        std::string description, 
        std::string new_version, 
        std::string window_manager,
        std::string hash,
        DependencyVec new_dependencies);

    void download_toml(ProgressBar *pb);
    bool verify_toml();
    void parse_toml();
    void install_git(ProgressBar *pb, int &rice_index, int rice_count);
    void install_desktop();
    static Rice from_string(std::string &database_line);

    short install_state;
    const std::string name;
    const std::string description;
    const std::string version;
    const std::string window_manager;
    const std::string hash;
    const std::string toml_path;
    const std::string toml_tmp_path;
    const std::string git_path;
    DependencyVec old_dependencies;
    DependencyVec new_dependencies; 

    std::string desktop_path;
    std::string git_repo_uri;
    std::string git_commit_hash;

    enum {
        NOT_INSTALLED     = 0b0000,
        TOML_INSTALLED    = 0b0001,
        GIT_INSTALLED     = 0b0010,
        DESKTOP_INSTALLED = 0b0100,
        UP_TO_DATE        = 0b1000,
    };

    private:

    std::string get_desktop_path(std::shared_ptr<cpptoml::table> rice_config);
    
    std::chrono::high_resolution_clock::time_point start_time;
    std::string wm_path;
    std::string wm_params;
};