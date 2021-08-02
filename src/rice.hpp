#pragma once

#include "progressbar.hpp"
#include "constants.hpp"
#include "utils.hpp"

#include <fmt/format.h>
#include <cpr/cpr.h>
#include <git2.h>
#include <cpptoml.h>

#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

typedef struct progress_data {
	git_indexer_progress fetch_progress;
	size_t completed_steps;
	size_t total_steps;
	const char *path;
} progress_data;

typedef struct Dependency {
    bool aur;
    std::string name;
} Dependency;

class Rice
{
    public:

    Rice(std::string name, 
        std::string id, 
        std::string description, 
        std::string version, 
        std::string window_manager,
        std::vector<Dependency> dependencies, 
        bool installed);

    void install();

    bool installed;
    const std::string name;
    const std::string id;
    const std::string description;
    const std::string version;
    const std::string window_manager;
    const std::string toml_path;
    const std::string git_path;
    std::vector<Dependency> dependencies; 

    std::string git_repo_uri;
    std::string git_commit_hash;

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

};