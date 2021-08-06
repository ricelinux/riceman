#include "package_manager.hpp"

#include <algorithm>
#include <iostream>

void PackageManager::install_diff(DependencyVec &old_deps, DependencyVec &new_deps)
{
    DependencyVec remove_deps;
    DependencyVec add_deps;

    std::sort(old_deps.begin(), old_deps.end());
    std::sort(new_deps.begin(), new_deps.end());

    auto diff_func = [](const Dependency &a, const Dependency &b) {
        return (a.name < b.name) || a.aur != b.aur;
    };

    std::set_difference(
        old_deps.begin(), old_deps.end(),
        new_deps.begin(), new_deps.end(),
        back_inserter(remove_deps),
        diff_func
    );

    std::set_difference(
        new_deps.begin(), new_deps.end(),
        old_deps.begin(), old_deps.end(),
        back_inserter(add_deps),
        diff_func
    );
    
    for (Dependency &dep : remove_deps) remove(dep);
    for (Dependency &dep : add_deps) install(dep);
}

