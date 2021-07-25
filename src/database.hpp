#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <fstream>

class Database
{
    public:
    Database(std::string localpath, std::string remoteuri);

    const std::string get_remote_hash();
    const std::string get_local_hash();

    const std::string local_path;
    const std::string remote_uri;

};

#endif // DATABASE_HPP