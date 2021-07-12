#include "util.hpp"
#include "config.hpp"

#include <filesystem>

namespace fs = std::filesystem;

const std::string basename(const std::string &path)
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
const bool log(const int level, const std::string &message)
{
	if ((config->loglevel & level) == 0) return false;
	
	switch(level)
	{
		case LOG_ERROR:
			std::cout << config->colors.err << "error: " << config->colors.nocolor; 
			break;
		case LOG_WARNING:
			std::cout << config->colors.warn << "warning: " << config->colors.nocolor; 
			break;
		case LOG_DEBUG:
			std::cout << "debug: ";
			break;
	}

	std::cout << message << std::endl;

	return true;
}

void colon_log(const std::string &message)
{
	std::cout << config->colors.colon << message << config->colors.nocolor << std::endl;
}

/** Check if the directory exists and to make sure its a directory
 * @param path The path to a directory or file
 * 
 * @returns 1 if a dir and able to read and write to, -1 if not a dir, -2 if doesn't exist
 */
int dir_exists(std::string &path)
{
	
	if (fs::exists(path))
	{
		if (!fs::is_directory(path)) return -1;
	} else return -2;

	return 1;
}