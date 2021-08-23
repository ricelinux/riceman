#pragma once

#include "rice.hpp"

#include <string>

class Database
{
    public:
    Database(std::string name, std::string remoteuri);

    Rice* get_rice(std::string name);
    const short refresh(std::string expected_hash);


    const std::string name;
    const std::string local_path;
    const std::string local_tmp_path;
    const std::string remote_uri;
    bool downloaded;
};