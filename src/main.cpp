#include "constants.hpp"
#include "utils.hpp"
#include "database_collection.hpp"

#include "sync.hpp"
#include "remove.hpp"

#include "argparse/argparse.hpp"
#include <fmt/format.h>

using argparse::ArgumentParser;

RicemanConfig config;
Utils utils{config};

static const struct option<int> op_opts[] =
{
    OP_OPT('S', "sync",     OP_SYNC,    "syncs rices"),
    OP_OPT('R', "remove",   OP_REMOVE,  "removes rices"),
    OP_OPT('Q', "query",    OP_QUERY,   "queries rices"),
};

/** Set the config's operation and manage error cases
 * @param argparser the ArgumentParser
 * 
 * @returns true if successfully set, false if bad format
 */
bool set_op(ArgumentParser &argparser)
{
    int argval = OP_UNSET;
    try {
        for(int i = 0; i < STRUCT_LEN(op_opts); i++) {
            /* If arg is set (either implicitly or not) */
            if ((argval = argparser.get<int>(op_opts[i].shortopt)) != 0) {
                if (argparser.get_length(op_opts[i].shortopt) > 1) return false;
                if (config.op != OP_UNSET) return false;
                config.op = argval;
            }
        }
    } catch (const std::runtime_error& err) {
        utils.log(LOG_FATAL, err.what());
    } catch (const std::logic_error& err) {
        utils.log(LOG_FATAL, err.what());
        exit(EXIT_FAILURE);
    }
    return true;
}

int main(int argc, char *argv[])
{
    OperationHandler* ophandler;
    /* I can't pass in escaped string macros without VSCode Makefile Tools getting mad */
    ArgumentParser argparser{"riceman", "v1.0.0"};
    DatabaseCollection databases;

    databases.add("master", REMOTE_BASE_URI);
    config.show_colors(true);
    
    bool ret;

    ADD_ARGUMENTS(argparser, op_opts);
    ADD_ARGUMENTS(argparser, SyncHandler::op_modifiers);
    //ADD_ARGUMENTS(argparser, RemoveHandler::op_modifiers);

    argparser.add_argument("targets").remaining();

    try {
        argparser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        utils.log(LOG_FATAL, err.what());
    } catch (const std::logic_error& err) {
        utils.log(LOG_FATAL, err.what());
    }
    
    /* If attempting to set operation fails */
    if (!set_op(argparser)) {
        utils.log(LOG_FATAL, "too many operations specified");
    }

    switch(config.op)
    {
        case OP_SYNC:
            NEEDS_ROOT(utils)
            ophandler = new SyncHandler(argparser, config, utils, databases);
            break;
        case OP_REMOVE:
            NEEDS_ROOT(utils);
            //ophandler = new RemoveHandler(argparser, config, utils, databases);
            break;
        case OP_QUERY:
            break;
        default:
            utils.log(LOG_FATAL, "no operation specified");
    }

    ophandler->run();

    Utils::show_cursor(true);
    Utils::handle_signals(false);

    return EXIT_SUCCESS;
}