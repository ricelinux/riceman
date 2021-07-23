#include "sync.hpp"
#include "constants.hpp"

/* Init statics */
const struct option<int> SyncHandler::op_modifiers[] = {
    OPT('u', "upgrade", 0, 1, "upgrades installed rices"),
};

const struct option<int> SyncHandler::compound_op_modifiers[] = { 
    OPT('y', "refresh", 0, 1, "refreshes downloaded databases"),
};
unsigned short SyncHandler::refresh = 0;

/* Main Code */
SyncHandler::SyncHandler(argparse::ArgumentParser &argparser, RicemanConfig &config, Utils &utils) 
: OperationHandler(argparser, config, utils)
{
    if (argparser.is_used("--refresh")) {
        refresh = argparser.get_length("--refresh");
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

    return true;
}