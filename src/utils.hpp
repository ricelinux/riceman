#pragma once

#include "config.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <csignal>

#include <unistd.h>

#include <cpr/cpr.h>
#include <fmt/format.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>

#define NEEDS_ROOT(utils) if (geteuid() != 0) { utils.log(LOG_FATAL, "you must be root to perform this operation"); }

using CryptoPP::byte, CryptoPP::SHA256;

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
    static const std::string get_file_content(const std::string &path);
    static const std::string get_uri_content(const std::string &uri);
    static const std::string hash_sha256(const std::string &content);
    static const std::string hash_file(const std::string &path);

    RicemanConfig &config;

    private:

    static void handle_sigint(int signum);
};