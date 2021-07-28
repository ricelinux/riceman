#pragma once

#include <string>

class ProgressBar
{
    public:

    ProgressBar(const std::string &title, double percent_length);

    void update(std::string prefix, double percentage);
    static std::string format_prefix_module(double bytes);

    private:

    void set_column(int col);

    unsigned int progress_start_pos;
    int progress_length;
    unsigned short window_width;

    const std::string &title;

};