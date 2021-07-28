#pragma once

#include <unistd.h>

#include "config.hpp"

#define NEEDS_ROOT(utils) if (geteuid() != 0) { utils.log(LOG_FATAL, "you must be root to perform this operation"); }

class Utils
{
    public:

    Utils(RicemanConfig &conf);

    const bool log(const int level, const std::string &message);
    const bool log(const int level, const int &message);
    void colon_log(const std::string &message);
    static void show_cursor(const bool status);
    static void handle_signals(const bool status);
    static const std::string basename(const std::string &path);

    RicemanConfig &config;

    private:

    static void handle_sigint(int signum);
};