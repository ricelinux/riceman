#pragma once

#include "option.hpp"
#include "operation.hpp"

class SyncHandler: public OperationHandler
{
    public:

    SyncHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils &util, DatabaseCollection &databases);
    bool run();

    static const int s_op_modifiers = 2;
    static const struct option<int> op_modifiers[s_op_modifiers];

    private:

    bool refresh_rices(unsigned short &level);

    /* Operation Modifiers */
    unsigned short refresh;
    bool upgrade;
};