#include "package_manager.hpp"

#include <algorithm>
#include <iostream>

void PackageManager::install_diff(DependencyVec &old_deps, DependencyVec &new_deps)
{
    DependencyVec intersection;

    std::sort(old_deps.begin(), old_deps.end());
    std::sort(new_deps.begin(), new_deps.end());

    std::set_intersection(
        old_deps.begin(), old_deps.end(),
        new_deps.begin(), new_deps.end(),
        back_inserter(intersection),
        [](const Dependency &a, const Dependency &b) {
            return (a.name < b.name) || a.aur != b.aur;
        }
    );
}