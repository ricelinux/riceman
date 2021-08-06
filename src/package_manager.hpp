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

class PackageManager
{
    public:

    static void install_diff(DependencyVec &old_deps, DependencyVec &new_deps);

    private:

    PackageManager();

    static void install(Dependency &dep);
    static void install_pacman(Dependency &dep);
    static void install_aur(Dependency &dep);

    static void remove(Dependency &dep);
    
    static void exec_pacman(char **args);
};