#ifndef OPTION_HPP
#define OPTION_HPP

#include <string>

#define OPT(shortopt, longopt, defaultval, implicitval, help) { std::string{'-', shortopt}, "--" longopt, defaultval, implicitval, help }
#define OP_OPT(shortopt, longopt, implicitval, help) OPT(shortopt, longopt, false, implicitval, help)

template <typename T>
struct option {
    const std::string shortopt;
    const std::string longopt;
    const T defaultval;
    const T implicitval;
    const std::string help;
};

#endif // OPTION_HPP