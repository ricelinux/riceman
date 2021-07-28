#include "progressbar.hpp"

#include <iostream>
#include <cmath>

#include <fmt/format.h>

#include <sys/ioctl.h>
#include <unistd.h>

ProgressBar::ProgressBar(const std::string &title, double percent_length): title{title}
{
    struct winsize window;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);

    std::cout << "\r " << title;

    /* NOTE: the 7 is the length of the '[]', percent symbol, space after progress, and percent */
    progress_length = std::trunc(window.ws_col * percent_length) - 7;
    progress_start_pos = (window.ws_col - progress_length) - 7;
    window_width = window.ws_col;

    update("", 0);
}

void ProgressBar::update(std::string prefix, double percentage)
{
    /* Trim leading white space */
    prefix.erase(0, prefix.find_first_not_of(" "));

    /* Reset position */
    std::cout << "\r";
    
    if (prefix.length() + title.length() > window_width) {
        std::cout << prefix;
    } else {
        /* print prefix text, set position to start of progress bar */
        set_column(progress_start_pos - prefix.length());
        std::cout << prefix;
        set_column(progress_start_pos);
        
        /* print progress bar */
        int completed_char_count = std::trunc(progress_length * percentage);

        std::cout << "[";
        for (int i = 0; i < progress_length; ++i) {
            if (i <= completed_char_count) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] "; 

        percentage *= 100;

        /* print percentage */
        if (percentage < 10) std::cout << "  ";
        else if (percentage < 100) std::cout << " ";

        std::cout << percentage << "%";
    }
}

void ProgressBar::set_column(int col)
{
    std::cout << "\r\033[" << col << "C";
}


std::string ProgressBar::format_prefix_module(double bytes)
{
    std::string unit{"B"};
    std::string spaces{};
    if (bytes > 1000000000) { // THE FUTURE IS NOW OLD MAN
        bytes /= 1000000000;
        unit = "GiB";
    } else if (bytes > 1000000) {
        bytes /= 1000000;
        unit = "MiB";
    } else if (bytes > 1000) {
        bytes /= 1000;
        unit = "KiB";        
    }

    std::string bytes_string = fmt::format("{:.1f}", bytes);

    int module_length = unit.length() + bytes_string.length();
    for(int i = 0; i < 10 - module_length; ++i) {
        spaces.append(" ");
    }
    return fmt::format("{0}{1} {2}", spaces, bytes_string, unit);
}