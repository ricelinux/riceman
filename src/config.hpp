#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "constants.hpp"
#include <string>

typedef struct _riceman_colors
{
    std::string colon = ":: ";
	std::string title = "";
	std::string repo = "";
	std::string version = "";
	std::string groups = "";
	std::string meta = "";
	std::string warn = "";
	std::string err = "";
	std::string faint = "";
	std::string nocolor = "";

} RicemanColors;

typedef struct _riceman_config
{
    int op = OP_UNSET;
    int loglevel = LOG_ERROR | LOG_WARNING;
    int color = COLOR_UNSET;
    
    bool verbose = false;
    bool version = false;
    bool help = false;
	bool noconfirm = false;
    bool noprogressbar = false;
    RicemanColors colstr;

} RicemanConfig;

#endif // CONFIG_HPP