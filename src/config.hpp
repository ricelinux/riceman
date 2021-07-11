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

typedef struct _riceman_sync_opts
{
	int refresh = 0;

	bool upgrade = false;
} RicemanSyncOpts;

typedef struct _riceman_remove_opts
{
	/* to be done when implementing remove functionality */
} RicemanRemoveOpts;

typedef struct _riceman_query_opts
{
	/* to be done when implementing query functionality */
} RicemanQueryOpts;

typedef struct _riceman_config
{
    int op = OP_UNSET;
    int loglevel = LOG_ERROR | LOG_WARNING;
    int color = COLOR_UNSET;
    
	RicemanColors colors;

	RicemanSyncOpts sync;
	RicemanQueryOpts query;
	RicemanRemoveOpts remove;

	bool needs_root = false;
    bool verbose = false;
    bool version = false;
    bool help = false;
	bool noconfirm = false;
    bool noprogressbar = false;
} RicemanConfig;

// Make config global
extern RicemanConfig *config;

#endif // CONFIG_HPP