#pragma once

#include "option.hpp"
#include "operation.hpp"
#include "constants.hpp"

class RemoveHandler: public OperationHandler
{
    public:

    RemoveHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils &util, DatabaseCollection &databases);
    bool run();

    static const int op_modifiers_s = 2;
    static const struct option<int> op_modifiers[op_modifiers_s];

    std::vector<std::string> targets;

    /* Operation Modifiers */
    bool remove_git;
    bool remove_toml;
    bool keep_desktop;
};