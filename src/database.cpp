#include "database.hpp"
#include "constants.hpp"

#include <fstream>
#include <sstream>
#include <filesystem>

#include <cpr/cpr.h>
#include <fmt/format.h>

#include <iostream> //TODO: remove this

using CryptoPP::byte, CryptoPP::SHA256;

namespace fs = std::filesystem;

Database::Database(std::string localpath, std::string remoteuri)
: local_path{localpath}, remote_uri{remoteuri}, remote_hash_uri{fmt::format("{}.sha256sum", remoteuri)}
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

const bool Database::refresh()
{
    using namespace std::placeholders;

    cpr::Response r = cpr::Get(cpr::Url{remote_uri},
                      cpr::ProgressCallback(std::bind(&Database::progress_callback, this, _1, _2, _3, _4)));
    

    std::ofstream stream;
    stream.open(local_path, std::fstream::out);
    if (!stream.is_open()) return false;

    stream.write(r.text.c_str(), r.text.length());
    return true;
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
    r.text.erase(std::remove(r.text.begin(), r.text.end(), '\n'), r.text.end());
    return r.text;
}

const std::string Database::get_local_hash()
{
    if (!local_exists() && !create_local()) throw std::runtime_error{fmt::format("unable to access {}", local_path)};
    std::ifstream file{local_path};
    std::ostringstream ss;

    ss << file.rdbuf();

    return hash_sha256(ss.str());
}

bool Database::progress_callback(size_t downloadTotal, size_t downloadNow, size_t uploadTotal, size_t uploadNow)
{
    std::cout << downloadNow << "/" << downloadTotal << std::endl;
    return true;
}