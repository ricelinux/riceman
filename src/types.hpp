#include <string>

template <typename T>
struct option {
    const std::string shortopt;
    const std::string longopt;
    const T defaultval;
    const T implicitval;
    const std::string help;
};