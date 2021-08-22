#pragma once

#include <string>
#include <vector>

#define PACMAN "/usr/bin/pacman"
#define DEFAULT_PACMAN_SYNC_ARG_LEN 4
#define DEFAULT_PACMAN_REMOVE_ARG_LEN 3
#define AUR_PKGBUILD_BASE "https://aur.archlinux.org/cgit/aur.git/plain/PKGBUILD?h="

typedef struct Dependency {
    bool aur;
    std::string name;

    bool operator <(const Dependency &comp) {
        return name < comp.name;
    };

    bool operator ==(const Dependency &comp) {
        return name == comp.name && aur == comp.aur;
    }
} Dependency;

typedef std::vector<Dependency> DependencyVec;

typedef struct DependencyDiff {
    DependencyVec add;
    DependencyVec remove;
} DependencyDiff;

class PackageManager
{
    public:

    static DependencyDiff get_diff(DependencyVec &old_deps, DependencyVec &new_deps);
    static void install(DependencyVec &deps);
    static void remove(DependencyVec &deps, std::vector<int> ignore_indexes);
    static int parse_ignore(std::string &ignore, std::vector<int> *vec);

    private:

    PackageManager();

    static void install_aur(std::vector<std::string> &deps);
};