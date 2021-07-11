#include "progressbar.hpp"

#include <iostream>

ProgressBar::ProgressBar(std::string title, const char pchar)
{
    title = " " + title;
    std::cout << title << std::endl;
}

int ProgressBar::update_progress(double dltotal, double dlnow, double ultotal, double ulnow)
{
    std::cout << dlnow << "/" << dltotal << std::endl;
    return 0;
}