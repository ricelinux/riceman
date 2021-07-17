#include "config.hpp"

#include "constants.hpp"

RicemanConfig::RicemanConfig()
    : op{OP_UNSET}, loglevel{LOG_ALL | LOG_ERROR | LOG_WARNING}, color{COLOR_UNSET}
{}

bool RicemanConfig::show_colors(bool state)
{
    if (state) {
        colors.colon    = BOLDBLUE "::" BOLD " ";
        colors.title    = BOLD;
        colors.repo     = BOLDMAGENTA;
        colors.version  = BOLDGREEN;
        colors.groups   = BOLDBLUE;
        colors.meta     = BOLDCYAN;
        colors.warn     = BOLDYELLOW;
        colors.err      = BOLDRED;
        colors.faint    = GREY46;
        colors.nocolor  = NOCOLOR;
    } else {
        colors.colon    = ":: ";
        colors.title    = "";
        colors.repo     = "";
        colors.version  = "";
        colors.groups   = "";
        colors.meta     = "";
        colors.warn     = "";
        colors.err      = "";
        colors.faint    = "";
        colors.nocolor  = "";
    }
    
}