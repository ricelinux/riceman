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

Database::Database(std::string name, std::string remote_uri)
: name{name}, local_path{fmt::format("{}/{}.db", LOCAL_DB_DIR, name)}, remote_uri{remote_uri}, local_tmp_path{fmt::format("{}.tmp", local_path)}, downloaded{false}
{
    downloaded = fs::exists(local_path) && fs::is_regular_file(local_path);
}


/** Downloads a fresh copy of the database
 * @param expected_hash the hash to be used for verifying the downloaded database
 * 
 * @returns -1 if request fails -2 if write fails, -3 if integrity check fails, 1 if successful
 */
const short Database::refresh(std::string expected_hash)
{
    using namespace std::placeholders;

    ProgressBar progress_bar{" " + name, 0.4};
    cpr::Response r = cpr::Get(cpr::Url{fmt::format("{}/rices.db", remote_uri)}, cpr::ProgressCallback(std::bind(&ProgressBar::progress_callback_download, &progress_bar, _1, _2, _3, _4)));
    progress_bar.done();

    if (r.error) return -1;

    if (!Utils::write_file_content(local_tmp_path, r.text)) return -2;

    /* Verify newly downloaded DB */
    if (Utils::hash_file(local_tmp_path).compare(expected_hash) == 0) fs::rename(local_tmp_path, local_path);
    else return -3; /* Integrity check failed */

    return 1;
}

Rice Database::get_rice(std::string &name)
{
    std::ifstream file{local_path};
    if (file.is_open()) {
        for(std::string line; std::getline(file, line); ) {
            if (line.length() == 0) continue;
            if (line.substr(0, name.size()) == name) 
                return Rice::from_string(line);
        }
    }
    throw std::runtime_error{""};
}