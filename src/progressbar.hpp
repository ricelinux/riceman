#ifndef PROGRESSBAR_HPP
#define PROGRESSBAR_HPP

#include <string>

class ProgressBar
{
    public:
        ProgressBar(std::string title, const char pchar);
        int update_progress(double dltotal, double dlnow, double ultotal, double ulnow);

    private:
        char progress_char;
};

#endif // PROGRESSBAR_HPP