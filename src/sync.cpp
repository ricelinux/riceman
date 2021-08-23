#include "sync.hpp"

#include <filesystem>

/* Init statics */
const struct option<int> SyncHandler::op_modifiers[SyncHandler::op_modifiers_s] = { 
    OPT('y', "refresh", 0, 1, "refreshes downloaded databases"),
    OPT('u', "upgrade", 0, 1, "upgrades installed rices")
};

namespace fs = std::filesystem;

SyncHandler::SyncHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils &util, DatabaseCollection &database_col) 
: OperationHandler(parser, conf, util, database_col), install{true}
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
SyncHandler::~SyncHandler()
{
    git_libgit2_shutdown();
}

void SyncHandler::run()
{
    if (refresh) {
        refresh_rices();
        install = false;
    }
    if (upgrade) {
        upgrade_rices();
        install = false;
    }
    if (!targets.empty() || install) {

        /* Verify to-be-installed rices */
        for(std::string &target : targets) {
            try {
                Rice *rice = databases.get_rice(target);
                if (rice != NULL) rices.push_back(rice);
            } catch (std::runtime_error) {
                incorrect_rice_names.push_back(target);
            }
        }

        install_rices(upgrade == 0);
    }
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
            remote_hash = Utils::get_uri_content(fmt::format("{}/rices.db.sha256sum", db.remote_uri));
        } catch (std::runtime_error err) {
            utils.log(LOG_FATAL, err.what());
        }
        
        if (local_hash.compare(remote_hash) != 0 || refresh == 2) {
            switch(db.refresh(remote_hash)) {
                case -3:
                    utils.log(LOG_FATAL, fmt::format("download of '{}' database corrupted", db.name));
                    break;
                case -2:
                    utils.log(LOG_FATAL, "failed to write database to file");
                    break;
                case -1:
                    utils.log(LOG_FATAL, "failed to download database");
                    break;
            }
        } else {
            std::cout << " " << db.name << " is up to date\n";
        }
    }

    Utils::show_cursor(true);
    Utils::handle_signals(false);

    return true;
}

/** This method first verifies the targets, then installs the installable rices, 
 *  and finally outputs an error at the end containing the rices unable to be installed
 */
bool SyncHandler::install_rices(bool hide_title)
{
    if (targets.size() == 0) {
        utils.log(LOG_FATAL, "no targets specified");
    }

    std::string adding_dep_str;
    std::string removing_dep_str;
    std::vector<DependencyDiff> dep_changes;

    if (rices.size() > 0) {
        /* Resolve new and outdated dependencies */
        for (Rice *rice : rices) {
            DependencyDiff diff = PackageManager::get_diff(rice->old_dependencies, rice->new_dependencies);
            
            for (Dependency &rm_dep : diff.remove) {
                removing_dep_str.append(rm_dep.name + " ");
            }

            for (Dependency &add_dep : diff.add) {
                adding_dep_str.append(add_dep.name + " ");
            }
            
            dep_changes.push_back(diff);
        }

        /* Log reinstall */
        for(Rice *rice : rices) {
            if ((rice->install_state & Rice::UP_TO_DATE) != 0) {
                utils.log(LOG_WARNING, fmt::format("{}-{} is up to date -- reinstalling", rice->name, rice->version));
            }
        }

        /* Print confirmation dialog */
        if (hide_title) utils.colon_log("Installing rices...");
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
        for (Rice *rice : rices) {
            ProgressBar pb{fmt::format(" {}{}-{}{}", rice->name, config.colors.faint, rice->version, config.colors.nocolor), 0.4};
            try {
                rice->download_toml(&pb);
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
            if (!rices[i]->verify_toml()) utils.log(LOG_FATAL, fmt::format("download of rice '{}' corrupted", rices[i]->name));
        }
        pb.done();

        /* Parse toml */
        pb = ProgressBar{fmt::format("(0/{}) parsing configuration files", rices.size()), 0.4};
        for (int i = 0; i < rices.size(); ++i) {
            pb.update_title(fmt::format("({}/{}) parsing configuration files", i+1, rices.size()));
            pb.update("", (double)i / (double)rices.size());

            try {
                rices[i]->parse_toml();
            } catch (std::runtime_error err) {
                utils.log(LOG_FATAL, err.what());
            }
        }
        pb.done();
        
        /* Move .toml.tmp to .toml */
        for (Rice *rice : rices) {
            try {
                std::filesystem::rename(rice->toml_tmp_path, rice->toml_path);
            } catch (std::filesystem::filesystem_error err) {
                utils.log(LOG_FATAL, fmt::format("unable to write to '{}'", rice->toml_path));
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

        ProgressBar{fmt::format("({0}/{0}) installing dependencies", rices.size()), 0.4}.done();

        utils.colon_log("Removing outdated dependencies...");

        for (int i = 0; i < rices.size(); ++i) {
            DependencyDiff &diff = dep_changes[i];
            
            if (diff.remove.size() > 0) {
                std::vector<int> ignore_indexes = utils.remove_confirmation_dialog(diff.remove);

                PackageManager::remove(diff.remove, ignore_indexes);
            } else utils.log(LOG_ALL, fmt::format(" nothing to remove for {}", rices[i]->name));
        }

        utils.colon_log("Processing changes...");

        /* Clone git repo */
        pb = ProgressBar{fmt::format("(0/{}) installing rices", rices.size()), 0.4};
        for(int i = 0; i < rices.size(); ++i) {
            pb.update_title(fmt::format("({}/{}) installing rices", i+1, rices.size()));
            pb.update("", (double)i / (double)rices.size());

            try {
                rices[i]->install_git(&pb, i, rices.size());
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
                rices[i]->install_desktop();
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
    
    for (int i = 0; i < incorrect_rice_names.size(); ++i) {
        utils.log(LOG_ERROR, "target not found: " + incorrect_rice_names[i]);
    }

    for (Database db : databases.db_list) {
        if (!db.downloaded) 
            utils.log(LOG_ERROR, fmt::format("{} is not downloaded (use -Sy to download)", db.name));
    }

    return true;
}

bool SyncHandler::upgrade_rices()
{
    utils.colon_log("Upgrading all rices...");
    for (Database &db : databases.db_list) {
        std::ifstream file{db.local_path};
        if (file.is_open()) {
            for (std::string line; std::getline(file, line); ) {
                if (line.length() == 0) continue;
                std::string new_version;
                std::string toml_path;
                std::string name;
                int i = 0;
                
                // todo: there has to be a better way to handle this
                std::stringstream line_stream{line};
                for (std::string value; std::getline(line_stream, value, ','); ) {
                    if (i == 0) {
                        name = value;
                    } else if (i == 1) {
                        toml_path = fmt::format("{}/{}.toml", LOCAL_CONFIG_DIR, value);
                    } else if (i == 3) {
                        new_version = value;
                        break;
                    }
                    i++;
                }
                
                /* Verify toml exists */
                if (!fs::exists(toml_path) || !fs::is_regular_file(toml_path)) continue;

                try {
                    /* Parse toml */
                    auto rice_config = cpptoml::parse_file(toml_path);
                    std::string old_version = rice_config->get_qualified_as<std::string>("theme.version").value_or("");
                    if (old_version == "" || old_version == new_version) continue;

                    /* Add to targets array */
                    targets.push_back(name);

                } catch (cpptoml::parse_exception &err) {
                    utils.log(LOG_WARNING, err.what());
                }
            }
        } else utils.log(LOG_WARNING, fmt::format("unable to access '{}' database", db.name));
        /* Above code could be a permissions issue, but it could also be a system which hasn't synced ever */
    }

    if (targets.size() == 0) {
        utils.log(LOG_ALL, " there is nothing to do");
        return false;
    } else return true;
}