#pragma once

/* Macros */
#define STRUCT_LEN(s) sizeof(s) / sizeof(s[0])

/* Setup fmt */
#define FMT_HEADER_ONLY

/* Version */
#ifndef VERSION
#define VERSION "v1.0.0"
#endif

/* Repository Info */
#define DB_NAME "rices.db"

#define REMOTE_BASE_URI "https://ricelinux.github.io/ricelinux-rices"
#define REMOTE_RICES_URI REMOTE_BASE_URI "/rices" 
#define REMOTE_DB REMOTE_BASE_URI "/" DB_NAME

#define LOCAL_BASE_DIR "/var/lib/riceman"		
#define LOCAL_DB_DIR LOCAL_BASE_DIR "/database"		/* Where all databases are written */
#define LOCAL_CONFIG_DIR LOCAL_BASE_DIR "/rices"	/* Where all rice .toml's are written */

#define LOCAL_RICES_DIR "/usr/share/rices"

/* Display server session paths */
#define XSESSIONS_PATH "/usr/share/xsessions"
#define WSESSIONS_PATH "/usr/share/wayland-sessions"

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

/* Log levels */
enum {
	LOG_ALL 		= 1,
	LOG_FATAL		= (1 << 1),
	LOG_ERROR 		= (1 << 2),
	LOG_WARNING		= (1 << 3),
	LOG_DEBUG		= (1 << 4)
};

/* Color Option Level */
enum {
	COLOR_UNSET = 1,
	COLOR_ENABLED,
	COLOR_DISABLED,
};
