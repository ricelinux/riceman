#pragma once

#include "database_rice.hpp"
#include "package_manager.hpp"

class LocalRice : public DatabaseRice {
    
    public:

    DependencyVec toml_dependencies;

    LocalRice(std::string name);
    LocalRice(DatabaseRice &rice);
};