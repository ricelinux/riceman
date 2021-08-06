#pragma once

#include <string>
#include <vector>

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
    static void remove(DependencyVec &deps);

    private:

    PackageManager();

    static void install_aur(std::vector<std::string> &deps);
    
    static void exec(char * const *args);
};