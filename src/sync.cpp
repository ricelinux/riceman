#include "sync.hpp"

#include <filesystem>

/* Init statics */
const struct option<int> SyncHandler::op_modifiers[SyncHandler::s_op_modifiers] = { 
    OPT('y', "refresh", 0, 1, "refreshes downloaded databases"),
    OPT('u', "upgrade", 0, 1, "upgrades installed rices")
};

SyncHandler::SyncHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils &util, DatabaseCollection &database_col) 
: OperationHandler(parser, conf, util, database_col)
{
    git_libgit2_init();
    
    if (argparser.is_used("--refresh")) {
        refresh = argparser.get_length("--refresh");
    }
    if (argparser.is_used("--upgrade")) {
        upgrade = argparser.get<int>("--upgrade");
    }
    if (argparser.is_used("targets")) {
        targets = argparser.get<std::vector<std::string>>("targets");
    }
}

bool SyncHandler::run()
{
    if (refresh) refresh_rices();
    if (upgrade) upgrade_rices();
    if (!targets.empty()) {

        /* Verify to-be-installed rices */
        for(std::string &target : targets) {
            try {
                rices.push_back(databases.get_rice(target));
            } catch (std::runtime_error) {
                incorrect_rice_names.push_back(target);
            }
        }

        install_rices();
    }

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

    std::string adding_dep_str;
    std::string removing_dep_str;
    std::vector<DependencyDiff> dep_changes;

    if (rices.size() > 0) {
        /* Resolve new and outdated dependencies */
        for (Rice &rice : rices) {
            DependencyDiff diff = PackageManager::get_diff(rice.old_dependencies, rice.new_dependencies);
            
            for (Dependency &rm_dep : diff.remove) {
                removing_dep_str.append(rm_dep.name + " ");
            }

            for (Dependency &add_dep : diff.add) {
                adding_dep_str.append(add_dep.name + " ");
            }
            
            dep_changes.push_back(diff);
        }

        /* Log reinstall */
        for(Rice &rice : rices) {
            if ((rice.install_state & Rice::UP_TO_DATE) != 0) {
                utils.log(LOG_WARNING, fmt::format("{}-{} is up to date -- reinstalling", rice.name, rice.version));
            }
        }

        /* Print confirmation dialog */
        utils.colon_log("Installing rices...");
        utils.rice_log(rices);
        
        std::cout << config.colors.title << "New Dependencies:\t"
                << config.colors.nocolor << (adding_dep_str.length() ? adding_dep_str : "None") << std::endl
                << config.colors.title << "Outdated Dependencies:\t" 
                << config.colors.nocolor << (removing_dep_str.length() ? removing_dep_str : "None") << std::endl 
                << std::endl;

        utils.colon_log("Proceed with installation? [Y/n] ", false);
        
        const char confirm = std::getchar();
        if (confirm != '\n' && confirm != 'y' && confirm != 'Y' && confirm != ' ') utils.log(LOG_FATAL, "install aborted");

        Utils::show_cursor(false);
        Utils::handle_signals(true);

        /* Download toml */
        for (Rice &rice : rices) {
            ProgressBar pb{fmt::format(" {}{}-{}{}", rice.name, config.colors.faint, rice.version, config.colors.nocolor), 0.4};
            try {
                rice.download_toml(&pb);
            } catch (std::runtime_error err) {
                utils.log(LOG_FATAL, err.what());
            }
            pb.done();
        }
        
        /* Check integrity */
        ProgressBar pb{fmt::format("(0/{}) checking integrity", rices.size()), 0.4};
        for (int i = 0; i < rices.size(); ++i) {
            pb.update_title(fmt::format("({}/{}) checking integrity", i+1, rices.size()));
            pb.update("", (double)i / (double)rices.size());
            if (!rices[i].verify_toml()) utils.log(LOG_FATAL, fmt::format("download of rice '{}' corrupted", rices[i].name));
        }
        pb.done();

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
        
        /* Move .toml.tmp to .toml */
        for (Rice &rice : rices) {
            try {
                std::filesystem::rename(rice.toml_tmp_path, rice.toml_path);
            } catch (std::filesystem::filesystem_error err) {
                utils.log(LOG_FATAL, fmt::format("unable to write to '{}'", rice.toml_path));
            }
        }

        utils.colon_log("Installing dependencies...");

        for (int i = 0; i < rices.size(); ++i) {
            DependencyDiff &diff = dep_changes[i];
            try { 
                PackageManager::install(diff.add);
            } catch (std::runtime_error err) {
                utils.log(LOG_FATAL, err.what());
            }
        }

        ProgressBar{"(1/1) installing dependencies", 0.4}.done();

        utils.colon_log("Removing outdated dependencies...");

        for (int i = 0; i < rices.size(); ++i) {
            DependencyDiff &diff = dep_changes[i];
            
            if (diff.remove.size() > 0) {
                std::string ignore;
                std::vector<int> ignore_indexes;

                std::cout << std::endl;

                for (int i = 0; i < diff.remove.size(); ++i) {
                    std::cout << config.colors.groups << i+1 << config.colors.nocolor << " " << config.colors.title << diff.remove[i].name << config.colors.nocolor << std::endl;
                }
                utils.colon_log("These packages are no longer in use and will be removed. Ignore any? [N]", true, false);
                utils.colon_log("[N]one [A]ll or (1 2 3)", true, false);
                std::cout << config.colors.version << ">> " << config.colors.nocolor;

                Utils::show_cursor(true);
                std::getline(std::cin, ignore);
                Utils::show_cursor(false);

                if (PackageManager::parse_ignore(ignore, &ignore_indexes) != 0) continue;

                for (int index: ignore_indexes) {
                    if (index <= diff.remove.size()) std::cout << "(1/1) skipping " << diff.remove[index - 1].name << std::endl;
                }

                try { 
                    PackageManager::remove(diff.remove, ignore_indexes);
                } catch (std::runtime_error err) {
                    utils.log(LOG_FATAL, err.what());
                }
            } else utils.log(LOG_ALL, fmt::format(" nothing to remove for {}", rices[i].name));
        }

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
    } 

    Utils::show_cursor(true);
    Utils::handle_signals(false);

    /* Deal with invalid rice names */
    if (incorrect_rice_names.size() == 0) return true;
    
    utils.log(LOG_ERROR, fmt::format("target not found:"), false);
    for (int i = 0; i < incorrect_rice_names.size(); ++i) {
        utils.log(LOG_ALL, " " + incorrect_rice_names[i], false);
    }
    std::cout << std::endl;

    return true;
}

bool SyncHandler::upgrade_rices()
{
    for (Database &db : databases.db_list) {
        for (Rice &rice : db.rices) {
            try {
                auto rice_config = cpptoml::parse_file(rice.toml_path);
            } catch (cpptoml::parse_exception err) {
                utils.log(LOG_ERROR, err.what());
            }
            
        }
    }

    return install_rices();
}