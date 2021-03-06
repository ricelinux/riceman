#pragma once

#include "config.hpp"
#include "constants.hpp"
#include "database_rice.hpp"

#include <unistd.h>

#include <cryptopp/sha.h>
#include <cryptopp/hex.h>

#define NEEDS_ROOT(utils) if (geteuid() != 0) { utils.log(LOG_FATAL, "you must be root to perform this operation"); }

/* Declare log level enum */
enum {
	LOG_ALL 		= 0b00001,
	LOG_FATAL		= 0b00010,
	LOG_ERROR 		= 0b00100,
	LOG_WARNING		= 0b01000,
	LOG_DEBUG		= 0b10000,
};

/* Declare color states enum */
enum {
    COLOR_UNSET = 1,
    COLOR_ENABLED,
    COLOR_DISABLED,
};
class Utils
{
    public:

    Utils(RicemanConfig &conf);

    const bool log(const int level, const std::string &message, const bool line_break = true);
    const bool log(const int level, const int &message, const bool line_break = true);
    void colon_log(const std::string &message, const bool line_break = true, const bool bold = true);
    void rice_log(const std::vector<DatabaseRice> &rices);
    std::vector<int> remove_confirmation_dialog(DependencyVec &deps);
    static void show_cursor(const bool status);
    static void handle_signals(const bool status);
    static const std::string basename(const std::string &path);
    static std::string get_file_content(const std::string &path);
    static const bool write_file_content(const std::string &path, const std::string &content);
    static void create_directory(const std::string &path);
    static void create_riceman_directories();
    static void own_directory(const std::string &path);
    static const std::string get_uri_content(const std::string &uri);
    static const std::string hash_sha256(const std::string &content);
    static const std::string hash_file(const std::string &path);
    static void exec(char * const *args, const char * working_dir, bool drop_root = false);

    RicemanConfig &config;

    private:

    static void handle_sigint(int signum);

};