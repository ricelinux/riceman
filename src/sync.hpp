#pragma once

#include "option.hpp"
#include "operation.hpp"
#include "constants.hpp"

#include <fmt/format.h>

class SyncHandler: public OperationHandler
{
    public:

    SyncHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils &util, DatabaseCollection &databases);
    ~SyncHandler();
    bool run();

    static const int s_op_modifiers = 2;
    static const struct option<int> op_modifiers[s_op_modifiers];

    private:

    bool refresh_rices();
    bool install_rices();
    bool upgrade_rices();

    std::vector<std::string> targets;
    std::vector<std::string> incorrect_rice_names;
    std::vector<Rice> rices;

    /* Operation Modifiers */
    unsigned short refresh;
    bool upgrade;
};