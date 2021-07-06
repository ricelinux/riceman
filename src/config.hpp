#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "constants.hpp"

struct RicemanColors {

    RicemanColors() : colon{":: "}, title{""}, repo{""}, version{""}, groups{""}, meta{""}, warn{""}, err{""}, faint{""}, nocolor{""} {}

    std::string colon;
	std::string title;
	std::string repo;
	std::string version;
	std::string groups;
	std::string meta;
	std::string warn;
	std::string err;
	std::string faint;
	std::string nocolor;
};

struct RicemanConfig
{
    RicemanConfig() : op{OP_UNSET}, color{false}, verbose{false}, 
                    version{false}, help{false}, noconfirm{false}, 
                    noprogressbar{false}, colstr{}, loglevel(LOG_ERROR | LOG_WARNING) {}

    int op;
    int loglevel;

    bool color;
    bool verbose;
    bool version;
    bool help;
	bool noconfirm;
    bool noprogressbar;
    RicemanColors colstr;
};

RicemanConfig* config;

#endif // CONFIG_HPP