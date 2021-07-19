#ifndef SYNC_HPP
#define SYNC_HPP

#include "operation.hpp"

class SyncHandler: public OperationHandler
{
    public:

    SyncHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils util);

    bool run();
};

#endif