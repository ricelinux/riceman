#include "config.hpp"

#include "constants.hpp"

RicemanConfig::RicemanConfig()
    : op{OP_UNSET}, loglevel{LOG_ALL | LOG_ERROR | LOG_WARNING}, color{COLOR_UNSET}
{}