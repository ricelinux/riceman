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
    utils.colon_log("Synchronizing rice databases...");
    
    for(int i = 0; i < databases.db_list.size(); i++) {
        Database &db = databases.get(i);

        std::cout << " " << db.db_name; 

        std::string local_hash;
        std::string remote_hash;

        try {
            local_hash = db.get_local_hash();
            remote_hash = db.get_remote_hash();
        } catch (std::runtime_error err) {
            std::cout << "\n";
            utils.log(LOG_ERROR, err.what());
            exit(EXIT_FAILURE);
        }
        
        if (local_hash.compare(remote_hash) != 0 || level == 2) {
            switch(db.refresh()) {
                case -2:
                    utils.log(LOG_ERROR, "failed to write database to file");
                    exit(EXIT_FAILURE);
                    break;
                case -1:
                    utils.log(LOG_ERROR, "failed to download database");
                    exit(EXIT_FAILURE);
                    break;
            }
        } else {
            std::cout << " is up to date\n";
        }
    }

    return true;
}