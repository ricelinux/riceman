#pragma once

#include <string>
#include <vector>

typedef struct Dependency {
    bool aur;
    std::string name;
} Dependency;

typedef std::vector<Dependency> DependencyVec;

class PackageManager
{
    public:

    static void install_diff(DependencyVec &depsa, DependencyVec &depsb);

    private:

    PackageManager();

    static void install_pacman(Dependency &dep);
    static void install_aur(Dependency &dep);
};