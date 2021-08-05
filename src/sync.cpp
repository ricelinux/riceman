#include "sync.hpp"

/* Init statics */
const struct option<int> SyncHandler::op_modifiers[SyncHandler::s_op_modifiers] = { 
    OPT('y', "refresh", 0, 1, "refreshes downloaded databases"),
    OPT('u', "upgrade", 0, 1, "upgrades installed rices")
};

SyncHandler::SyncHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils &util, DatabaseCollection &database_col) 
: OperationHandler(parser, conf, util, database_col), install{true}
{
    git_libgit2_init();
    
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

        std::string local_hash;
        std::string remote_hash;

        try {
            local_hash = Utils::hash_file(db.local_path);
            remote_hash = Utils::get_uri_content(db.remote_hash_uri);
        } catch (std::runtime_error err) {
            std::cout << "\n";
            utils.log(LOG_FATAL, err.what());
        }
        
        if (local_hash.compare(remote_hash) != 0 || refresh == 2) {
            switch(db.refresh(remote_hash)) {
                case -3:
                    utils.log(LOG_FATAL, fmt::format("download of '{}' database corrupted", db.db_name));
                    break;
                case -2:
                    utils.log(LOG_FATAL, "failed to write database to file");
                    break;
                case -1:
                    utils.log(LOG_FATAL, "failed to download database");
                    break;
            }
        } else {
            std::cout << " " << db.db_name << " is up to date\n";
        }
    }

    Utils::show_cursor(true);
    Utils::handle_signals(false);

    return true;
}

/** This method first verifies the targets, then installs the installable rices, 
 *  and finally outputs an error at the end containing the rices unable to be installed
 */
bool SyncHandler::install_rices()
{
    if (targets.size() == 0) {
        utils.log(LOG_FATAL, "no targets specified");
    }

    std::vector<Rice> rices;
    std::vector<std::string> incorrect_rice_names; 

    /* Verify to-be-installed rices */
    for (int i = 0; i < targets.size(); ++i) {
        std::string &target = targets[i];
        try {
            rices.push_back(databases.get_rice(target));
        } catch (std::runtime_error err) {
            incorrect_rice_names.push_back(target);
        }
    }

    /** Install all available rices **/
    for(int i = 0; i < rices.size(); ++i) {
        /* If rice is not up-to-date */
        if ((rices[i].install_state & Rice::UP_TO_DATE) != 0) {
            utils.log(LOG_WARNING, fmt::format("{}-{} is up to date -- reinstalling", rices[i].name, rices[i].version));
        }
    }

    utils.colon_log("Installing rices...");
    utils.rice_log(rices);
    utils.colon_log("Proceed with installation? [Y/n] ", false);
    
    const char confirm = std::getchar();
    if (confirm != '\n' && confirm != 'y' && confirm != 'Y' && confirm != ' ') utils.log(LOG_FATAL, "install aborted");

    Utils::show_cursor(false);
    Utils::handle_signals(true);

    for (int i = 0; i < rices.size(); ++i) {
        try {
            rices[i].download_toml(fmt::format("{}{}-{}{}", rices[i].name, config.colors.faint, rices[i].version, config.colors.nocolor));
        } catch (std::runtime_error err) {
            utils.log(LOG_FATAL, err.what());
        }
    }
    
    /* Check integrity */
    ProgressBar pb{fmt::format("(0/{}) checking integrity", rices.size()), 0.4};
    for (int i = 0; i < rices.size(); ++i) {
        pb.update_title(fmt::format("({}/{}) checking integrity", i+1, rices.size()));
        pb.update("", (double)i / (double)rices.size());
        if (!rices[i].verify_toml()) utils.log(LOG_FATAL, fmt::format("download of rice '{}' corrupted", rices[i].name));
    }
    pb.done();

    /* Install deps */
    for (int i = 0; i < rices.size(); ++i) {
        rices[i].install_deps();
    }

    /* Parse toml */
    pb = ProgressBar{fmt::format("(0/{}) parsing configuration files", rices.size()), 0.4};
    for (int i = 0; i < rices.size(); ++i) {
        pb.update_title(fmt::format("({}/{}) parsing configuration files", i+1, rices.size()));
        pb.update("", (double)i / (double)rices.size());

        try {
            rices[i].parse_toml();
        } catch (std::runtime_error err) {
            utils.log(LOG_FATAL, err.what());
        }
    }
    pb.done();

    utils.colon_log("Processing changes...");

    /* Clone git repo */
    pb = ProgressBar{fmt::format("(0/{}) installing rices", rices.size()), 0.4};
    for(int i = 0; i < rices.size(); ++i) {
        pb.update_title(fmt::format("({}/{}) installing rices", i+1, rices.size()));
        pb.update("", (double)i / (double)rices.size());

        try {
            rices[i].install_git();
        } catch (std::runtime_error err) {
            utils.log(LOG_FATAL, err.what());
        }
    }
    pb.done();

    /* Write .desktop files to session dir */
    pb = ProgressBar{fmt::format("(0/{}) writing desktop entries", rices.size()), 0.4}; 
    for (int i = 0; i < rices.size(); ++i) {
        pb.update_title(fmt::format("({}/{}) writing desktop entries", i+1, rices.size()));
        pb.update("", (double)i / (double)rices.size());

        try {
            rices[i].install_desktop();
        } catch (std::runtime_error err) {
            utils.log(LOG_FATAL, err.what());
        }
    }

    pb.done();

    Utils::show_cursor(true);
    Utils::handle_signals(false);

    /* Deal with invalid rice names */
    if (incorrect_rice_names.size() == 0) return true;
    
    utils.log(LOG_ERROR, fmt::format("target not found:"), false);
    for(int i = 0; i < incorrect_rice_names.size(); ++i) {
        utils.log(LOG_ALL, " " + incorrect_rice_names[i], false);
    }
    std::cout << std::endl;

    return true;
}