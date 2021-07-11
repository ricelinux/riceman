#ifndef UTIL_HPP
#define UTIL_HPP

#include <string.h>
#include <string>
#include <iostream>

const std::string basename(const std::string &path);
const bool log(const int level, const std::string &message);
const bool colon_log(const std::string &message);

#endif // UTIL_HPP