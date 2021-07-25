#include "sync.hpp"
#include "constants.hpp"

/* Init statics */
const struct option<int> SyncHandler::op_modifiers[SyncHandler::s_op_modifiers] = { 
    OPT('y', "refresh", 0, 1, "refreshes downloaded databases"),
    OPT('u', "upgrade", 0, 1, "upgrades installed rices")
};

SyncHandler::SyncHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils &util, DatabaseCollection &database_col) 
: OperationHandler(parser, conf, util, database_col)
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
    if (refresh) refresh_rices(refresh);

    if (upgrade) {
        utils.log(LOG_ALL, "Upgrading all rices.");
    }

    return true;
}

/* Backend code */
bool SyncHandler::refresh_rices(unsigned short &level)
{
    std::vector<Database> &db_list = databases.db_list;

    for(int i = 0; i < db_list.size(); i++)
    {
        Database &db = db_list[i];

        if (level == 1) {

        } else {
            utils.log(LOG_ALL, db.get_remote_hash());
        }
    }
}