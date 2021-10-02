#include "installed_rice.hpp"
#include "constants.hpp"
#include "package_manager.hpp"

#include <cpptoml.h>
#include <fmt/format.h>

InstalledRice::InstalledRice(std::string namearg) {
    auto toml = cpptoml::parse_file(fmt::format("{}/{}.toml", LOCAL_CONFIG_DIR, namearg));

    std::vector<std::string> pacman_deps = toml->get_qualified_array_of<std::string>("packages.pacman").value_or((std::vector<std::string>){});
    std::vector<std::string> aur_deps = toml->get_qualified_array_of<std::string>("packages.aur").value_or((std::vector<std::string>){});
    DependencyVec deps;

    for (std::string dep : pacman_deps) deps.push_back({ false, dep });
    for (std::string dep : aur_deps) deps.push_back({ true, dep });

    DatabaseRice(
        toml->get_qualified_as<std::string>("theme.name").value_or(""),
        toml->get_qualified_as<std::string>("theme.description").value_or(""),
        toml->get_qualified_as<std::string>("theme.version").value_or(""),
        toml->get_qualified_as<std::string>("window-manager.name").value_or(""),
        "", // No hash available
        deps
    );
}

InstalledRice::InstalledRice(DatabaseRice &rice) {
    DatabaseRice(rice);
}