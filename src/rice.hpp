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
        std::string id, 
        std::string description, 
        std::string new_version, 
        std::string window_manager,
        std::string hash,
        DependencyVec new_dependencies);

    void download_toml(ProgressBar *pb);
    bool verify_toml();
    void parse_toml();
    void install_git();
    void install_desktop();

    int install_state;
    const std::string name;
    const std::string id;
    const std::string description;
    const std::string version;
    const std::string window_manager;
    const std::string hash;
    const std::string toml_path;
    const std::string toml_tmp_path;
    const std::string git_path;
    DependencyVec old_dependencies;
    DependencyVec new_dependencies; 

    std::string git_repo_uri;
    std::string git_commit_hash;

    enum {
        NOT_INSTALLED  = 0b000,
        TOML_INSTALLED = 0b001,
        GIT_INSTALLED  = 0b010,
        UP_TO_DATE     = 0b100,
        BOTH_INSTALLED = TOML_INSTALLED | GIT_INSTALLED,
    };

    private:

    const bool repo_exists(const std::string &path, git_repository **repo);
    const std::string get_head_hash(git_repository *repo);
    void handle_libgit_error(int error);

    static void checkout_progress(const char *path, size_t cur, size_t tot, void *payload);
    static int fetch_progress(const git_indexer_progress *stats, void *payload);
    static void cred_acquire(git_credential **out,
		const char *url,
		const char *username_from_url,
		unsigned int allowed_types,
		void *payload);

    std::chrono::high_resolution_clock::time_point start_time;
    std::string display_server;
    std::string wm_path;
    std::string wm_params;
};