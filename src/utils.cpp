#include "utils.hpp"
#include "constants.hpp"

#include <iostream>

Utils::Utils(RicemanConfig &conf)
    : config{conf} {};

/** Get the name of the file or folder the path points to
 * @param path Path to a file or folder
 * 
 * @return The name of the file or folder that the path points to
 */
const std::string Utils::basename(const std::string &path)
{
    std::string::size_type namepos = path.rfind('/');
	
	/* If rfind returned an error */
	if (namepos != std::string::npos) ++namepos;
	else namepos = 0;

	return path.substr(namepos);
}

/** Check if config allows for certain level of logging and log accordingly
 * @param level Log level defined by enum in constants.hpp
 * @param message The string to log
 * 
 * @return A boolean, true if logged, false if not logged
 */
const bool Utils::log(const int level, const std::string &message)
{
	if ((config.loglevel & level) == 0) return false;
	
	switch(level)
	{
		case LOG_ERROR:
			std::cout << config.colors.err << "error: " << config.colors.nocolor; 
			break;
		case LOG_WARNING:
			std::cout << config.colors.warn << "warning: " << config.colors.nocolor; 
			break;
		case LOG_DEBUG:
			std::cout << "debug: ";
			break;
        case LOG_ALL:
			std::cout << config.colors.nocolor;
            /* Nothing done here */
            break;
        default:
            return false;
	}

	std::cout << message << std::endl;

	return true;
}

const bool Utils::log(const int level, const int &message)
{
	return log(level, std::to_string(message));
}

void Utils::colon_log(const std::string &message)
{
	std::cout << config.colors.colon << message << config.colors.nocolor << std::endl;
}

void Utils::show_cursor(const bool status)
{
	if (status) std::cout << "\033[?25h";
	else std::cout << "\033[?25l";
}