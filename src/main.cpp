#include "constants.hpp"
#include "config.hpp"
#include "utils.hpp"

#include <argparse/argparse.hpp>

typedef struct options {
    std::string shortopt;
    std::string longopt;
    std::string help;
} Options;

static const Options opts[] = {
    { "-S", "--sync", "sync rices" },
    { "-R", "--remove", "remove rices"},
    { "-Q", "--query", "query rices"}
};

int main(int argc, char *argv[])
{   
    argparse::ArgumentParser program{"riceman", TO_STRING(VERSION)};

    RicemanConfig config;
    Utils utils{config};

    for (int i = 0; i < sizeof(opts) / sizeof(opts[0]); i++ ) {
        program.add_argument(opts[i].shortopt, opts[i].longopt)
            .help(opts[i].help);
    }

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        utils.log(LOG_ERROR, err.what());
    }
}