#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include <git2.h>

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
    
    static void clone_progress(const progress_data *pd);
    static void cred_acquire(git_credential **out,
		const char *url,
		const char *username_from_url,
		unsigned int allowed_types,
		void *payload);

};