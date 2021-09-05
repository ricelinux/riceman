#pragma once

#include "package_manager.hpp"

#include <string>

typedef struct DatabaseRice {
    const std::string name;
    const std::string description;
    const std::string version;
    const std::string window_manager;
    const std::string hash;
    DependencyVec dependencies;
} DatabaseRice;