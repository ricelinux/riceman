#pragma once

#include <string>
#include <chrono>

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

class ProgressBar
{
    public:

    ProgressBar(const std::string &title, double percent_length);

    void update(std::string prefix, double percentage);
    void done();
    bool progress_callback_download(size_t downloadTotal, size_t downloadNow, size_t uploadTotal, size_t uploadNow);
    static std::string format_prefix_module(double bytes);

    private:

    void set_column(int col);

    unsigned int progress_start_pos;
    int progress_length;
    unsigned short window_width;

    std::chrono::high_resolution_clock::time_point start_time;

    const std::string &title;

};