#ifndef SYNC_HPP
#define SYNC_HPP

#include "option.hpp"
#include "operation.hpp"

class SyncHandler: public OperationHandler
{
    public:

    SyncHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils &util);
    bool run();

    static const int s_op_modifiers = 1;
    static const struct option<int> op_modifiers[s_op_modifiers];
    static const struct option<int> compound_op_modifiers[s_op_modifiers];

    private:

    /* Public Operation Modifiers */
    static unsigned short refresh;
};

#endif