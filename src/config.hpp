#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "constants.hpp"

struct RicemanColors {

    RicemanColors() : colon{":: "}, title{""}, repo{""}, version{""}, groups{""}, meta{""}, warn{""}, err{""}, faint{""}, nocolor{""} {}

    const char *colon;
	const char *title;
	const char *repo;
	const char *version;
	const char *groups;
	const char *meta;
	const char *warn;
	const char *err;
	const char *faint;
	const char *nocolor;
};

struct RicemanConfig
{
    RicemanConfig() : op{OP_MAIN}, color{false}, verbose{false}, version{false}, help{false}, noconfirm{false}, noprogressbar{false}, colstr{} {}

    int op;

    bool color;
    bool verbose;
    bool version;
    bool help;
	bool noconfirm;
    bool noprogressbar;
    RicemanColors colstr;
};

#endif // CONFIG_HPP