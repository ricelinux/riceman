#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP



/* Macros */
#define TO_STRING(s) #s
#define STRUCT_LEN(s) sizeof(s) / sizeof(s[0])

#define OPT(shortopt, longopt, defaultval, implicitval, help) { std::string{'-', shortopt}, "--" longopt, defaultval, implicitval, help }
#define OP_OPT(shortopt, longopt, implicitval, help) OPT(shortopt, longopt, false, implicitval, help)

/* Setup fmt */
#define FMT_HEADER_ONLY

/* Version */
#ifndef VERSION
#define VERSION "???"
#endif

/* Repository Info */
#define RICE_DB_NAME "rices.db"
#define RICE_DB_SUM_NAME RICE_DB_NAME ".sha256sum"

#define RICE_REPO "https://ricelinux.github.io/ricelinux-rices"
#define RICE_REPO_DB RICE_REPO "/" RICE_DB_NAME
#define RICE_REPO_DB_SUM RICE_REPO "/" RICE_DB_SUM_NAME

#define RICE_LIB_PATH "/var/lib/riceman"
#define RICE_DB_PATH RICE_LIB_PATH "/" RICE_DB_NAME
#define RICE_DB_SUM_PATH RICE_LIB_PATH "/" RICE_DB_SUM_NAME

/* Colors */
#define NOCOLOR       "\033[0m"

#define BOLD          "\033[0;1m"

#define BLACK         "\033[0;30m"
#define RED           "\033[0;31m"
#define GREEN         "\033[0;32m"
#define YELLOW        "\033[0;33m"
#define BLUE          "\033[0;34m"
#define MAGENTA       "\033[0;35m"
#define CYAN          "\033[0;36m"
#define WHITE         "\033[0;37m"

#define BOLDBLACK     "\033[1;30m"
#define BOLDRED       "\033[1;31m"
#define BOLDGREEN     "\033[1;32m"
#define BOLDYELLOW    "\033[1;33m"
#define BOLDBLUE      "\033[1;34m"
#define BOLDMAGENTA   "\033[1;35m"
#define BOLDCYAN      "\033[1;36m"
#define BOLDWHITE     "\033[1;37m"
#define GREY46        "\033[38;5;243m"

/* Cursor */
#define CURSOR_HIDE "\x1B[?25l"
#define CURSOR_SHOW "\x1B[?25h"


/* Operations */
enum {
	OP_UNSET = 1,
	OP_REMOVE,
	OP_UPGRADE,
	OP_QUERY,
	OP_SYNC,
};

/* Long Operations */
/* Note: set to 1000 so that there is a difference 
   	     between characters and op codes */
enum {
	OP_LONG_MIN = 1000,
	/* Sync opts */
	OP_REFRESH,
	OP_UPGRADES,

	/* Global Opts */
	OP_COLOR,
};

/* Log levels */
enum {
	LOG_ALL 		= 1,
	LOG_ERROR 		= (1 << 1),
	LOG_WARNING		= (1 << 2),
	LOG_DEBUG		= (1 << 3)
};

/* Color Option Level */
enum {
	COLOR_UNSET = 1,
	COLOR_ENABLED,
	COLOR_DISABLED,
};

#endif