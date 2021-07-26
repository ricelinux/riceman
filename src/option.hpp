#pragma once

#include <string>

#define OPT(shortopt, longopt, defaultval, implicitval, help) { std::string{'-', shortopt}, "--" longopt, defaultval, implicitval, help }
#define OP_OPT(shortopt, longopt, implicitval, help) OPT(shortopt, longopt, false, implicitval, help)

#define ADD_ARGUMENTS(argparser, opts) \
for (int i = 0; i < STRUCT_LEN(opts); i++) { \
    argparser.add_argument(opts[i].shortopt, opts[i].longopt) \
        .nargs(0) \
        .append() \
        .implicit_value(opts[i].implicitval) \
        .default_value(opts[i].defaultval) \
        .help(opts[i].help); \
}

template <typename T>
struct option {
    const std::string shortopt;
    const std::string longopt;
    const T defaultval;
    const T implicitval;
    const std::string help;
};