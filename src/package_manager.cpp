#include "package_manager.hpp"

#include <algorithm>
#include <iostream>

#include <unistd.h>
#include <sys/wait.h>

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

void PackageManager::install(Dependency &dep)
{
    if (dep.aur) install_aur(dep);
    else install_pacman(dep);
}

void PackageManager::install_pacman(Dependency &dep)
{

}

void PackageManager::install_aur(Dependency &dep)
{

}

void PackageManager::remove(Dependency &dep)
{

}

void PackageManager::exec_pacman(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();

    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            throw std::runtime_error{"failed to fork child process"};
        }
    } else if (pid > 0) {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    } else {
        throw std::runtime_error{"failed to fork child process"};
    }


}