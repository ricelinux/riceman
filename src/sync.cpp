#include "sync.hpp"
#include "constants.hpp"

/* Init statics */
const struct option<int> SyncHandler::op_modifiers[SyncHandler::s_op_modifiers] = { 
    OPT('y', "refresh", 0, 1, "refreshes downloaded databases"),
    OPT('u', "upgrade", 0, 1, "upgrades installed rices")
};

SyncHandler::SyncHandler(argparse::ArgumentParser &argparser, RicemanConfig &config, Utils &utils) 
: OperationHandler(argparser, config, utils)
{
    if (argparser.is_used("--refresh")) {
        refresh = argparser.get_length("--refresh");
    }
    if (argparser.is_used("--upgrade")) {
        upgrade = argparser.get<int>("--upgrade");
    }
}

bool SyncHandler::run()
{
    switch (refresh)
    {
        case 0:
            utils.log(LOG_ALL, "Database will not be refreshed.");
            break;
        case 1:
            utils.log(LOG_ALL, "Database will be refreshed if there is a new version.");
            break;
        default:
            utils.log(LOG_ALL, "Database will be refreshed no matter what.");
            break;
    }

    if (upgrade) {
        utils.log(LOG_ALL, "Upgrading all rices.");
    }

    return true;
}