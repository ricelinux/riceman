#include "remove.hpp"

const struct option<int> RemoveHandler::op_modifiers[RemoveHandler::op_modifiers_s] = {
    OPT('g', "remove-git",      0,  1,  "removes the git repository"),
    OPT('t', "remove-toml",     0,  1,  "removes the rice config"),
    OPT('d', "keep-desktop",    0,  1,  "ignores the removal of the desktop entry")
};

RemoveHandler::RemoveHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils &util, DatabaseCollection &database_col)
: OperationHandler(parser, conf, util, database_col)
{
    remove_git   = argparser.is_used("--remove-git");
    remove_toml  = argparser.is_used("--remove-toml");
    keep_desktop = argparser.is_used("--keep-desktop");
}

bool RemoveHandler::run()
{
    std::cout << "Removing..." << std::endl;
}