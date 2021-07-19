#include "constants.hpp"


#include "config.hpp"
#include "utils.hpp"

#include "option.hpp"
#include "operation.hpp"
#include "sync.hpp"

#include <argparse/argparse.hpp>
#include <fmt/format.h>

using argparse::ArgumentParser;

RicemanConfig config;
Utils utils{config};

static const struct option<int> op_opts[] =
{
    OP_OPT('S', "sync",     OP_SYNC,    "syncs rices"),
    OP_OPT('R', "remove",   OP_REMOVE,  "removes rices"),
    OP_OPT('Q', "query",    OP_QUERY,   "queries rices")
};

bool set_op(ArgumentParser &argparser)
{
    int argval = OP_UNSET;
    try {
        for(int i = 0; i < STRUCT_LEN(op_opts); i++) {
            /* If arg is set (either implicitly or not) */
            if ((argval = argparser.get<int>(op_opts[i].shortopt)) != 0) {
                if (config.op != OP_UNSET) return false;
                config.op = argval;
            }
        }
    } catch (const std::runtime_error& err) {
        utils.log(LOG_ERROR, err.what());
        exit(EXIT_FAILURE);
    } catch (const std::logic_error& err) {
        utils.log(LOG_ERROR, err.what());
        exit(EXIT_FAILURE);
    }
    return true;
}



int main(int argc, char *argv[])
{   
    ArgumentParser argparser{"riceman", TO_STRING(VERSION)};
    bool ret;

    for (int i = 0; i < STRUCT_LEN(op_opts); i++) {
        argparser.add_argument(op_opts[i].shortopt, op_opts[i].longopt)
            .implicit_value(op_opts[i].implicitval)
            .default_value(op_opts[i].defaultval);
    }

    try {
        argparser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        utils.log(LOG_ERROR, err.what());
        exit(EXIT_FAILURE);
    } catch (const std::logic_error& err) {
        utils.log(LOG_ERROR, err.what());
        exit(EXIT_FAILURE);
    }
    
    /* If attempting to set operation fails */
    if (!set_op(argparser)) {
        utils.log(LOG_ERROR, "too many operations specified");
        exit(EXIT_FAILURE);
    }

    OperationHandler* ophandler;

    switch(config.op)
    {
        case OP_SYNC:
            ophandler = new SyncHandler(argparser, config, utils);
            break;
        default:
            utils.log(LOG_ERROR, "no operation specified");
            exit(EXIT_FAILURE);
    }

    ophandler->run();
}