#include "database.hpp"
#include "progressbar.hpp"
#include "utils.hpp"
#include "constants.hpp"

#include <fmt/format.h>
#include <cpr/cpr.h>

#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

Database::Database(std::string name, std::string remoteuri)
: db_name{name}, local_path{fmt::format("{}/{}.db", LOCAL_DB_DIR, name)}, remote_uri{remoteuri}, remote_hash_uri{fmt::format("{}/rices.db.sha256sum", remoteuri)}, local_tmp_path{fmt::format("{}.tmp", local_path)}, remote_db{fmt::format("{}/rices.db", remoteuri)}
{
    create_local();
    std::ifstream file{local_path};
    if (file.is_open()) {
        for(std::string line; std::getline(file, line); ) {
            std::stringstream line_stream{line};
            std::string rice_data[7];
            std::vector<Dependency> deps;
            int valuei = 0;
            
            for(std::string value; std::getline(line_stream, value, ','); ) {
                rice_data[valuei] = value;
                ++valuei;
            }

            std::stringstream deps_stream{rice_data[5]};
            for(std::string dependency; std::getline(deps_stream, dependency, ';'); ) {
                int slash_loc = dependency.find("/");
                bool aur;
                std::string type = dependency.substr(0, slash_loc);
                if (type.compare("pacman") == 0) aur = false;
                else if (type.compare("aur") == 0) aur = true;
                else throw std::runtime_error{fmt::format("unexpected dependecy type found in theme '{}'", rice_data[0])};

                deps.push_back({
                    aur,
                    dependency.substr(slash_loc + 1, dependency.length())
                });
            }

            rices.push_back(Rice(rice_data[0], rice_data[1], rice_data[2], rice_data[3], rice_data[4], rice_data[6], deps));
        }
    } 
    /* If not open, don't throw error because it could be newly added or recently removed */
    file.close();
}

const bool Database::create_local()
{
    std::ofstream stream;

    stream.open(local_path, std::fstream::app);
    if (!stream.is_open()) return false;
    stream.close();

    fs::permissions(local_path, fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read | fs::perms::others_read);
    
    return true;
}

/** Downloads a fresh copy of the database
 * @param expected_hash the hash to be used for verifying the downloaded database
 * 
 * @returns -1 if request fails -2 if write fails, -3 if integrity check fails, 1 if successful
 */
const short Database::refresh(std::string expected_hash)
{
    using namespace std::placeholders;

    ProgressBar progress_bar{db_name, 0.4};
    cpr::Response r = cpr::Get(cpr::Url{remote_db}, cpr::ProgressCallback(std::bind(&ProgressBar::progress_callback_download, &progress_bar, _1, _2, _3, _4)));
    progress_bar.done();

    if (r.error) return -1;

    if (!Utils::write_file_content(local_tmp_path, r.text)) return -2;

    /* Verify newly downloaded DB */
    if (Utils::hash_file(local_tmp_path).compare(expected_hash) == 0) fs::rename(local_tmp_path, local_path);
    else return -3; /* Integrity check failed */

    return 1;
}

Rice& Database::get_rice(std::string name)
{
    for (int i = 0; i < rices.size(); ++i) {
        if (rices[i].name.compare(name) == 0) return rices[i];
    }
    throw std::runtime_error{""}; /* No need to waste processing on formatting an error message since it won't be displayed */
}