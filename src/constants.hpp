#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

// Version
#ifndef VERSION
#define VERSION "???"
#endif

// Repository Info
#define RICE_REPO "https://ricelinux.github.io/ricelinux-rices/"
#define RICE_DB RICE_REPO "rices.db"

// Colors
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

// Operations
enum {
	OP_MAIN = 1,
	OP_REMOVE,
	OP_UPGRADE,
	OP_QUERY,
	OP_SYNC,
};

#endif // CONSTANTS_HPP