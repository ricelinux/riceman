#include "package_manager.hpp"
#include "utils.hpp"
#include "git_repository.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <filesystem>

#include <cpr/cpr.h>
#include <fmt/format.h>
#include <git2.h>

namespace fs = std::filesystem;

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
    std::vector<char *> pacman = { strdup(PACMAN), strdup("-S"), strdup("--noconfirm"), strdup("--needed") };
    std::vector<std::string> aur;

    for (Dependency &dep : deps) {
        if (dep.aur) aur.push_back(dep.name);
        else pacman.push_back(strdup(dep.name.data()));
    }

    pacman.push_back(NULL);

    if (pacman.size() > 5) Utils::exec(pacman.data(), NULL);
    if (aur.size() > 0) install_aur(aur);

    FREE_ARRAY(pacman);
}

void PackageManager::install_aur(std::vector<std::string> &deps)
{   
    /* Clone and chown git repo */
    for (std::string &dep : deps) {
        using namespace std::placeholders;

        std::string dep_path{fmt::format("{}/{}", AUR_INSTALL_DIR, dep)};
        std::string aur_repo_uri{fmt::format("{}/{}.git", AUR_BASE_URI, dep)};
        GitRepository repo{dep_path, aur_repo_uri};
        if (!repo.cloned) {
            try {
                repo.clone();
            } catch (std::runtime_error err) {
                std::filesystem::remove_all(dep_path);
                repo.clone(); /* Try to clone one more time, but if it fails just throw the error */
            } 
        }
        Utils::own_directory(dep_path);
    }

    /* Run makepkg */
    char * makepkg_args[] = {strdup("/usr/bin/makepkg"), strdup("-sf")};
    for (std::string &dep : deps) {
        Utils::exec(makepkg_args, fmt::format("{}/{}", AUR_INSTALL_DIR, dep).c_str(), true);
    }

    /* Find all package files */
    std::vector<char *> pacman_args = { strdup("/usr/bin/pacman"), strdup("-U"), strdup("--noconfirm") };

    for (std::string &dep : deps) {
        for (const auto & entry : fs::directory_iterator(fmt::format("{}/{}", AUR_INSTALL_DIR, dep))) {
            const std::string &name = entry.path().filename();
            if( name.length() > dep.length() + 11 &&
                name.substr(0, dep.length()) == dep &&
                name.substr(name.length() - 11, name.length()) == ".pkg.tar.gz") {
                pacman_args.push_back(strdup(entry.path().generic_string().data()));
                break;
            }
        }
    }

    pacman_args.push_back(NULL);

    for (char * arg : pacman_args) {
        std::cout << arg << " ";
    }
    std::cout << std::endl;

    /* Install all package files */
    Utils::exec(pacman_args.data(), NULL, false);

    /* Free all duped strings */
    FREE_ARRAY(pacman_args);
    FREE_ARRAY(makepkg_args);
}

void PackageManager::remove(DependencyVec &deps, std::vector<int> ignore_indexes)
{
    std::vector<char *> remove_args;
    remove_args.push_back(strdup(PACMAN));
    remove_args.push_back(strdup("-Rus"));
    remove_args.push_back(strdup("--noconfirm"));

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

    if (remove_args.size() > DEFAULT_PACMAN_REMOVE_ARG_LEN + 1) Utils::exec(remove_args.data(), NULL);
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