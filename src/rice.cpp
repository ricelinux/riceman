#include "rice.hpp"
#include "constants.hpp"

#include <fmt/format.h>

Rice::Rice(std::string name, std::string id, std::string description, std::string version, std::string window_manager, std::vector<Dependency> dependencies, bool installed)
: name{name}, id{id}, description{description}, version{version}, window_manager{window_manager}, dependencies{dependencies}, installed{installed}, file_path{fmt::format("{}/{}.toml", LOCAL_RICES_DIR, id)}
{}

