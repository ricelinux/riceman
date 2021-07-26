#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

typedef struct _riceman_colors {
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

typedef struct _riceman_sync_opts {
	int refresh = 0;

	bool upgrade = false;
} RicemanSyncOpts;

typedef struct _riceman_remove_opts {
	/* to be done when implementing remove functionality */
} RicemanRemoveOpts;

typedef struct _riceman_query_opts {
	/* to be done when implementing query functionality */
} RicemanQueryOpts;

class RicemanConfig
{
    public:

    RicemanConfig();

    void show_colors(bool state);
    
	int op;
    int loglevel;
    int color;
    
	RicemanColors colors;

	RicemanSyncOpts sync;
	RicemanQueryOpts query;
	RicemanRemoveOpts remove;

	bool needs_root;
	bool noconfirm;
    bool noprogressbar;
    
};

#endif // CONFIG_HPP