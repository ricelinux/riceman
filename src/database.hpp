#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <fstream>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>

using CryptoPP::byte;

class Database
{
    public:
    Database(std::string localpath, std::string remoteuri);

    const std::string get_remote_hash();
    const std::string get_local_hash();
    const bool update();

    const std::string local_path;
    const std::string remote_uri;
    const std::string remote_hash_uri;

    private:
    const std::string hash_sha256(std::string input);
    const bool local_exists();
    const bool create_local();
};

#endif // DATABASE_HPP