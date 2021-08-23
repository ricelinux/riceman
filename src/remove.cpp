#include "remove.hpp"

#include <fmt/format.h>

const struct option<int> RemoveHandler::op_modifiers[RemoveHandler::op_modifiers_s] = {
    OPT('g', "remove-git",      0,  1,  "removes the git repository"),
    OPT('t', "remove-toml",     0,  1,  "removes the rice config"),
    OPT('d', "keep-desktop",    0,  1,  "ignores the removal of the desktop entry"),
    OPT('c', "complete",        0,  1,  "removes the git repository, rice config, and desktop entry (overrides all other options)"),
};

RemoveHandler::RemoveHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils &util, DatabaseCollection &database_col)
: OperationHandler(parser, conf, util, database_col)
{
    if (argparser.is_used("complete")) {
        remove_git = true;
        remove_toml = true;
        remove_desktop = true;
    } else {
        remove_git      =  argparser.is_used("--remove-git");
        remove_toml     =  argparser.is_used("--remove-toml");
        remove_desktop  = !argparser.is_used("--keep-desktop");
    }

    if (argparser.is_used("targets")) {
        targets = argparser.get<std::vector<std::string>>("targets");
    }
}

void RemoveHandler::run()
{
    if (!targets.empty()) {

        /* Verify to-be-installed rices */
        for(std::string &target : targets) {
            try {
                rices.push_back(databases.get_rice(target));
            } catch (std::runtime_error err) {
                incorrect_rice_names.push_back(target);
            }
        }

        remove_rices();
    } else utils.log(LOG_FATAL, "no targets specified");
}

void RemoveHandler::remove_rices()
{
    utils.colon_log("Removing rices...");
    utils.rice_log(rices);

    for (Rice &rice : rices) {
        
    }
}