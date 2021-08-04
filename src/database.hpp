#pragma once

#include "progressbar.hpp"
#include "utils.hpp"
#include "rice.hpp"
#include "constants.hpp"

#include <fmt/format.h>

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <fstream>

class Database
{
    public:
    Database(std::string name, std::string remoteuri);

    Rice& get_rice(std::string name);
    const short refresh(std::string expected_hash);

    const std::string db_name;
    const std::string local_path;
    const std::string local_tmp_path;
    const std::string remote_uri;
    const std::string remote_db;
    const std::string remote_hash_uri;

    private:
    const bool create_local();
    
    std::vector<Rice> rices;
};