#include "util.hpp"
#include "config.hpp"

#include <dirent.h>
#include <unistd.h>

const std::string basename(const std::string &path)
{
	const char *last = strrchr(path.c_str(), '/');
	if(last) {
		return last + 1;
	}
	return path;
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

// Answered here: https://stackoverflow.com/a/24544478/8540492
int dir_exists(const char *path)
{
	DIR *dirptr;

	if (access ( path, F_OK ) != -1 ) {
        // file exists
        if ((dirptr = opendir (path)) != NULL) {
            closedir (dirptr); /* d exists and is a directory */
        } else {
            return -2; /* d exists but is not a directory */
        }
    } else {
        return -1;     /* d does not exist */
    }

	return 1;
}