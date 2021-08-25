#include "remove.hpp"

#include <filesystem>

#include <fmt/format.h>

namespace fs = std::filesystem;

const struct option<int> RemoveHandler::op_modifiers[RemoveHandler::op_modifiers_s] = {
    OPT('g', "keep-git",      0,  1,  "keeps the git repository"),
    OPT('t', "keep-toml",     0,  1,  "keeps the rice config (only use if you know what you're doing)"),
    OPT('d', "keep-desktop",  0,  1,  "keeps the desktop entry (only use if you know what you're dong)"),
};

RemoveHandler::RemoveHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils &util, DatabaseCollection &database_col)
: OperationHandler(parser, conf, util, database_col)
{
    remove_git      = !argparser.is_used("--keep-git");
    remove_toml     = !argparser.is_used("--keep-toml");
    remove_desktop  = !argparser.is_used("--keep-desktop");

    if (argparser.is_used("targets")) {
        targets = argparser.get<std::vector<std::string>>("targets");
    }
}

void RemoveHandler::run()
{
    if (!targets.empty()) {

        /* Verify to-be-installed rices */
        for(std::string &target : targets) {
            try {
                Rice rice = databases.get_rice(target);
                std::cout << rice.install_state << std::endl;
                if (rice.install_state != Rice::NOT_INSTALLED) rices.push_back(rice);
            } catch (std::runtime_error err) {
                incorrect_rice_names.push_back(target);
            }
        }

        remove_rices();
    } else utils.log(LOG_FATAL, "no targets specified");
}

void RemoveHandler::remove_rices()
{
    std::string dependency_string;
    utils.colon_log("Removing rices...");
    utils.rice_log(rices);

    for (Rice &rice : rices) {
        for (Dependency &dep : rice.old_dependencies) {
            dependency_string.append(dep.name).append(" ");
        }
    }

    std::cout << config.colors.title << "Removed Dependencies:\t"
            << config.colors.nocolor << (dependency_string.length() == 0 ? "None" : dependency_string) 
            << std::endl << std::endl;
    
    utils.colon_log("Proceed with removal? [Y/n] ", false);

    const char confirm = std::getchar();
    if (confirm != '\n' && confirm != 'y' && confirm != 'Y' && confirm != ' ') utils.log(LOG_FATAL, "removal aborted");

    Utils::show_cursor(false);
    Utils::handle_signals(true);

    utils.colon_log("Processing rice changes...");

    for (Rice &rice : rices) {
        ProgressBar pb{" " + rice.name, 0.4};

        try {
            if ((rice.install_state & Rice::DESKTOP_INSTALLED) != 0 && remove_desktop) 
                fs::remove(rice.desktop_path);
            pb.update("", 0.33);
        } catch (fs::filesystem_error) {
            utils.log(LOG_ERROR, fmt::format("failed to remove desktop entry for '{}'", rice.name));
        }

        try {
            if ((rice.install_state & Rice::GIT_INSTALLED) != 0 && remove_git) 
                fs::remove(rice.git_path);
            pb.update("", 0.66);
        } catch (fs::filesystem_error) {
            utils.log(LOG_ERROR, fmt::format("failed to remove git repository for '{}'", rice.name));
        }

        try {
            if ((rice.install_state & Rice::TOML_INSTALLED) != 0 && remove_toml) 
                fs::remove(rice.toml_path);
            pb.update("", 1);
        } catch (fs::filesystem_error) {
            utils.log(LOG_ERROR, fmt::format("failed to remove rice config for '{}'", rice.name));
        }

        pb.done();
    }

    utils.colon_log("Processing package changes...");

    for (Rice &rice : rices) {
        PackageManager::remove(rice.old_dependencies, utils.remove_confirmation_dialog(rice.old_dependencies));
    }

    Utils::show_cursor(true);
    Utils::handle_signals(false);
}