#include "rice.hpp"
#include "constants.hpp"

#include <fmt/format.h>
#include "git2.h"



Rice::Rice(std::string name, std::string id, std::string description, std::string version, std::string window_manager, std::vector<Dependency> dependencies, bool installed)
: name{name}, id{id}, description{description}, version{version}, window_manager{window_manager}, dependencies{dependencies}, installed{installed}, file_path{fmt::format("{}/{}.toml", LOCAL_TOML_DIR, id)}
{}

void Rice::install()
{
    CREATE_DIRECTORY(LOCAL_RICES_DIR);

    download_toml();
}

void Rice::download_toml()
{
    
}

void Rice::clone_progress(const git_transfer_progress *stats, void *payload)
{
    
}