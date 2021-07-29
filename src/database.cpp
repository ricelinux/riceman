#include "database.hpp"
#include "constants.hpp"

#include <fstream>
#include <sstream>
#include <filesystem>

#include <cpr/cpr.h>
#include <fmt/format.h>

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>

using CryptoPP::byte, CryptoPP::SHA256;

namespace fs = std::filesystem;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

Database::Database(std::string name, std::string remoteuri)
: db_name{name}, local_path{fmt::format("{}/{}.db", LOCAL_RICE_BASE_URI, name)}, remote_uri{remoteuri}, remote_hash_uri{fmt::format("{}.sha256sum", remoteuri)}, local_tmp_path{fmt::format("{}.tmp", local_path)}
{}

const bool Database::create_local()
{
    if (fs::exists(LOCAL_RICE_BASE_URI) && !fs::is_directory(LOCAL_RICE_BASE_URI)) fs::remove(LOCAL_RICE_BASE_URI);
    if (!fs::exists(LOCAL_RICE_BASE_URI)) {
        fs::create_directory(LOCAL_RICE_BASE_URI);
        fs::permissions(LOCAL_RICE_BASE_URI, fs::perms::owner_all | fs::perms::group_exec | fs::perms::group_read | fs::perms::others_read | fs::perms::others_exec);
    }

    std::ofstream stream;

    stream.open(local_path, std::fstream::out);
    if (!stream.is_open()) return false;
    stream.write("", 0);

    fs::permissions(local_path, fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read | fs::perms::others_read);
    
    return true;
}

const bool Database::local_exists()
{
    return std::filesystem::exists(local_path);
}

/** Downloads a fresh copy of the database
 * @param expected_hash the hash to be used for verifying the downloaded database
 * 
 * @returns -1 if request fails -2 if write fails, -3 if integrity check fails, 1 if successful
 */
const short Database::refresh(std::string expected_hash)
{
    using namespace std::placeholders;

    progress_bar = new ProgressBar(db_name, 0.4);
    start_time = Clock::now();
    cpr::Response r = cpr::Get(cpr::Url{remote_uri}, cpr::ProgressCallback(std::bind(&Database::progress_callback, this, _1, _2, _3, _4)));

    std::cout << std::endl;

    delete progress_bar;

    if (r.error) return -1;

    /* Save data to temp file */
    std::ofstream stream;
    stream.open(local_tmp_path, std::fstream::out);
    if (!stream.is_open()) return -2;
    stream.write(r.text.c_str(), r.text.length());
    stream.close();

    /* Verify newly downloaded DB */
    if (get_local_hash(local_tmp_path).compare(expected_hash) == 0) fs::rename(local_tmp_path, local_path);
    else return -3; /* Integrity check failed */

    return 1;
}

const std::string Database::hash_sha256(std::string input)
{
    SHA256 hash;
    byte digest[SHA256::DIGESTSIZE];

    hash.CalculateDigest(digest, (byte *)input.c_str(), input.length());
    
    CryptoPP::HexEncoder encoder;
    std::string output;
    encoder.Attach(new CryptoPP::StringSink(output));
    encoder.Put(digest, sizeof(digest));
    encoder.MessageEnd();

    return output;
}

const std::string Database::get_remote_hash()
{    
    cpr::Response r = cpr::Get(cpr::Url(remote_hash_uri));
    if (r.error) throw std::runtime_error{r.error.message};
    r.text.erase(std::remove(r.text.begin(), r.text.end(), '\n'), r.text.end());
    return r.text;
}

const std::string Database::get_local_hash()
{
    return get_local_hash(local_path);
}

const std::string Database::get_local_hash(std::string path)
{
    if (!local_exists() && !create_local()) throw std::runtime_error{fmt::format("unable to access {}", path)};
    std::ifstream file{path};
    std::ostringstream ss;

    ss << file.rdbuf();

    return hash_sha256(ss.str());
}

bool Database::progress_callback(size_t dtotal, size_t dnow, size_t utotal, size_t unow)
{
    milliseconds millis = std::chrono::duration_cast<milliseconds>(Clock::now() - start_time);
    double rate = (double)dnow / millis.count();
    double percentage = trunc((double)dnow / (double)(dtotal == 0 ? 1 : dtotal));
    std::string prefix = fmt::format("{} {}/s ", ProgressBar::format_prefix_module(dnow), ProgressBar::format_prefix_module(rate * 1000));
    
    progress_bar->update(prefix, percentage);

    return true;
}
