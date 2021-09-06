#include "database_rice.hpp"
#include "utils.hpp"

#include <cpr/cpr.h>
#include <fmt/format.h>
#include <filesystem>

namespace fs = std::filesystem;

DatabaseRice::DatabaseRice(std::string name, std::string description, std::string version, std::string window_manager, std::string hash, DependencyVec dependencies)
: name{name}, description{description}, version{version}, window_manager{window_manager}, dependencies{dependencies},
    toml_path{fmt::format("{}/{}.toml", LOCAL_CONFIG_DIR, name)}, git_path{fmt::format("{}/{}", LOCAL_RICES_DIR, name)}
{
    if (fs::exists(toml_path) && fs::is_regular_file(toml_path)) install_state = install_state | TOML_INSTALLED;
    if (fs::exists(git_path) && fs::is_directory(git_path)) install_state = install_state | GIT_INSTALLED;
}

void DatabaseRice::download_toml(std::string path, ProgressBar &pb)
{
    using namespace std::placeholders;
    cpr::Response r = cpr::Get(cpr::Url{fmt::format("{}/{}.toml", REMOTE_RICES_URI, name)}, cpr::ProgressCallback{std::bind(&ProgressBar::progress_callback_download, &pb, _1, _2, _3, _4)});

    /* Handle potential errors */
    if (r.error) throw std::runtime_error{fmt::format("{} (error code {})", r.error.message, r.error.code)};

    if (!Utils::write_file_content(path, r.text)) throw std::runtime_error{fmt::format("unable to write to '{}'", path)};
}

DatabaseRice DatabaseRice::from_string(std::string &db_line)
{
    std::stringstream line_stream{db_line};
    std::string rice_data[6];
    DependencyVec deps;
    int valuei = 0;
    
    /* Parse comma separated list into array */
    for(std::string value; std::getline(line_stream, value, ','); ) {
        rice_data[valuei] = value;
        ++valuei;
    }
    if (valuei != 6) throw std::runtime_error{"malformatted database string"};

    /* Parse dependencies */
    std::stringstream deps_stream{rice_data[4]};
    for(std::string dependency; std::getline(deps_stream, dependency, ';'); ) {
        int slash_loc = dependency.find("/");
        bool aur;
        std::string type = dependency.substr(0, slash_loc);
        if (type.compare("pacman") == 0) aur = false;
        else if (type.compare("aur") == 0) aur = true;
        else throw std::runtime_error{"incorrect dependency type in database string"};

        deps.push_back({
            aur,
            dependency.substr(slash_loc + 1, dependency.length())
        });
    }
    return DatabaseRice(rice_data[0], rice_data[1], rice_data[2], rice_data[3], rice_data[5], deps);
}