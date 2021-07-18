#include "constants.hpp"
#include "config.hpp"
#include "utils.hpp"

#include <argparse/argparse.hpp>

using argparse::ArgumentParser;

RicemanConfig config;
Utils utils{config};

bool set_config(ArgumentParser &argparser)
{
    try {
        /* Parse ops */
    } catch (const std::runtime_error& err) {
        utils.log(LOG_ERROR, err.what());
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{   
    ArgumentParser argparser{"riceman", TO_STRING(VERSION)};

    argparser.add_argument("-V", "--version")
        .default_value(false)
        .help("displays the version of riceman");
    
    argparser.add_argument("-S", "--sync")
        .help("sync rices");


    try {
        argparser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        utils.log(LOG_ERROR, err.what());
        exit(EXIT_FAILURE);
    }

    set_config(argparser);
}