#pragma once

#include <unistd.h>

#include "config.hpp"

#define NEEDS_ROOT(utils) if (geteuid() != 0) { utils.log(LOG_ERROR, "you must be root to perform this operation"); exit(EXIT_FAILURE); }

class Utils
{
    public:

    Utils(RicemanConfig &conf);

    const bool log(const int level, const std::string &message);
    const bool log(const int level, const int &message);
    void colon_log(const std::string &message);
    void show_cursor(const bool status);
    const std::string basename(const std::string &path);
    int dir_exists(const std::string &path);

    RicemanConfig &config;
};