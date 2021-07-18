
template <typename T>
struct option {
    const char shortopt;
    const std::string longopt;
    const T defaultval;
    const T implicitval;
};