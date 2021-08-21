#include "package_manager.hpp"
#include "utils.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <filesystem>

#include <cpr/cpr.h>
#include <fmt/format.h>
#include <git2.h>

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

    if (pacman.size() > 5) Utils::exec(pacman.data(), NULL);
    if (aur.size() > 0) install_aur(aur);
}

void PackageManager::install_aur(std::vector<std::string> &deps)
{
    for (std::string &dep : deps) {
        using namespace std::placeholders;

        std::string dep_path{fmt::format("{}/{}", AUR_INSTALL_DIR, dep)};
        try {
            Utils::create_directory(dep_path);
        } catch (std::filesystem::filesystem_error err) {
            throw (std::runtime_error)err;
        }

        ProgressBar pb{" " + dep, 0.4};
        cpr::Response r = cpr::Get(
            cpr::Url{fmt::format("{}{}", AUR_PKGBUILD_BASE, dep)}, 
            cpr::ProgressCallback{std::bind(&ProgressBar::progress_callback_download, pb, _1, _2, _3, _4)}
        );
        pb.done();

        if (r.error) throw std::runtime_error{fmt::format("{} (error code {})", r.error.message, r.error.code)};
        if (!Utils::write_file_content(dep_path.append("/PKGBUILD"), r.text)) throw std::runtime_error{fmt::format("unable to write to '{}'", dep_path)};
    }

    for (std::string &dep : deps) {
        char * makepkg_args[] = {"/usr/bin/makepkg", "-sf", "--verifysource", "--needed"};
        Utils::exec(makepkg_args, fmt::format("{}/{}", AUR_INSTALL_DIR, dep).c_str());
    }
}

void PackageManager::remove(DependencyVec &deps, std::vector<int> ignore_indexes)
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