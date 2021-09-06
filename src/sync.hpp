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
    void run();

    static const int op_modifiers_s = 2;
    static const struct option<int> op_modifiers[op_modifiers_s];

    private:

    bool refresh_rices();
    bool install_rices(bool hide_title = false);
    bool upgrade_rices();

    std::vector<std::string> targets;
    std::vector<std::string> incorrect_rice_names;
    std::vector<DatabaseRice> rices;

    /* Operation Modifiers */
    unsigned short refresh;
    bool upgrade;
    bool install;
};