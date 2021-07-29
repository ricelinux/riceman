#include "sync.hpp"
#include "constants.hpp"

#include <fmt/format.h>

/* Init statics */
const struct option<int> SyncHandler::op_modifiers[SyncHandler::s_op_modifiers] = { 
    OPT('y', "refresh", 0, 1, "refreshes downloaded databases"),
    OPT('u', "upgrade", 0, 1, "upgrades installed rices")
};

SyncHandler::SyncHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils &util, DatabaseCollection &database_col) 
: OperationHandler(parser, conf, util, database_col), install{true}
{
    if (argparser.is_used("--refresh")) {
        refresh = argparser.get_length("--refresh");
        install = false;
    }
    if (argparser.is_used("--upgrade")) {
        upgrade = argparser.get<int>("--upgrade");
        install = false;
    }
    if (argparser.is_used("targets")) {
        targets = argparser.get<std::vector<std::string>>("targets");
    }
}

bool SyncHandler::run()
{
    if (refresh) refresh_rices();
    if (upgrade) utils.log(LOG_ALL, "Upgrading all rices.");
    if (install) install_rices();

    return true;
}

/* Backend code */
bool SyncHandler::refresh_rices()
{
    utils.colon_log("Synchronizing rice databases...");

    Utils::show_cursor(false);
    Utils::handle_signals(true);
    
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
            utils.log(LOG_FATAL, err.what());
        }
        
        if (local_hash.compare(remote_hash) != 0 || refresh == 2) {
            switch(db.refresh(remote_hash)) {
                case -3:
                    utils.log(LOG_FATAL, fmt::format("download of '{}' database corrupted.\n       Temporary database still present in {} for debugging.", db.db_name, LOCAL_RICE_BASE_URI));
                case -2:
                    utils.log(LOG_FATAL, "failed to write database to file");
                    break;
                case -1:
                    utils.log(LOG_FATAL, "failed to download database");
                    break;
            }
        } else {
            std::cout << " is up to date\n";
        }
    }

    Utils::show_cursor(true);
    Utils::handle_signals(false);

    return true;
}

bool SyncHandler::install_rices()
{
    if (targets.size() == 0) {
        utils.log(LOG_FATAL, "no targets specified");
    }

    for (int i = 0; i < targets.size(); ++i)
    {
        std::string &target = targets[i];
        
    }

    return true;
}