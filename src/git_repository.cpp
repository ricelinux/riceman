#include "git_repository.hpp"

#include <filesystem>

namespace fs = std::filesystem;

GitRepository::GitRepository(std::string &path, std::string &remote)
    : path{path}, remote{remote}
{
    if (fs::exists(path) && fs::is_directory(path)) {
        int error = git_repository_open_ext(&repo, path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
        cloned = error >= 0;
    } else {
        cloned = false;
    }
}