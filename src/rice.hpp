#pragma once

#include <string>
#include <vector>

typedef struct Dependency {
    bool aur;
    std::string name;
} Dependency;

class Rice
{
    public:

    Rice(std::string name, std::string id, std::string description, std::string version, std::string window_manager, std::vector<Dependency> dependencies, bool installed);

    bool installed;
    const std::string name;
    const std::string id;
    const std::string description;
    const std::string version;
    const std::string window_manager;
    std::vector<Dependency> dependencies; 

};