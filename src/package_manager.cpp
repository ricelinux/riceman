#include "package_manager.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

#include <unistd.h>
#include <sys/wait.h>

DependencyDiff PackageManager::get_diff(DependencyVec &old_deps, DependencyVec &new_deps)
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
    
    return {
        add_deps,
        remove_deps,
    };
}

void PackageManager::install(DependencyVec &deps)
{
    std::vector<char *> pacman = { DEFAULT_PACMAN_SYNC_ARGS };
    std::vector<std::string> aur;

    for (Dependency &dep : deps) {
        if (dep.aur) aur.push_back(dep.name);
        else pacman.push_back(dep.name.data());
    }

    pacman.push_back(NULL);

    if (pacman.size() > 5) exec(pacman.data());
    if (aur.size() > 0) install_aur(aur);
}

void PackageManager::install_aur(std::vector<std::string> &deps)
{
    std::cout << "Installing all AUR packages:" << std::endl;
    for (std::string &dep : deps) {
        std::cout << dep << " ";
    }
}

void PackageManager::remove(DependencyVec &deps, std::vector<int> &ignore_indexes)
{
    std::vector<char *> remove_args = { DEFAULT_PACMAN_REMOVE_ARGS };

    if (ignore_indexes.size() > 0 && ignore_indexes[0] == -2) return;

    for (int index : ignore_indexes) {
        if (index <= deps.size()) {
            deps[index - 1].name = "\0";
        }
    }

    for (Dependency &dep : deps) {
        if (dep.name != "\0") remove_args.push_back(dep.name.data());
    }

    remove_args.push_back(NULL);

    if (remove_args.size() > DEFAULT_PACMAN_REMOVE_ARG_LEN + 1) exec(remove_args.data());
}

int PackageManager::parse_ignore(std::string &ignore, std::vector<int> *vec)
{
    if (ignore.length() == 0 || ignore[0] == 'n' || ignore[0] == 'N') return 0;
    else if (ignore[0] == 'a' || ignore[0] == 'A') return -1;

    std::stringstream ignore_stream{ignore};

    for (std::string value; std::getline(ignore_stream, value, ' ');) {
        try {
            vec->push_back(std::stoi(value));
        } catch (std::invalid_argument err) {}; /* do nothing if invalid */
    }

    return 0;
}

void PackageManager::exec(char * const *args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();

    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            throw std::runtime_error{"failed to run pacman in child process"};
        }
    } else if (pid > 0) {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    } else {
        throw std::runtime_error{"failed to fork child process"};
    }
}