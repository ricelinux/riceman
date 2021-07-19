#include "sync.hpp"
#include "constants.hpp"

SyncHandler::SyncHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils util)
    : OperationHandler(parser, conf, util) {};

bool SyncHandler::run()
{
    utils.log(LOG_ALL, std::to_string(config.op));
}