#ifndef UTILS_HPP
#define UTILS_HPP

#include "config.hpp"

class Utils
{
    public:

    Utils(RicemanConfig &conf);

    const bool log(const int level, const std::string &message);
    void colon_log(const std::string &message);
    const std::string basename(const std::string &path);
    int dir_exists(const std::string &path);

    private:

    RicemanConfig &config;
};

#endif // UTILS_HPP