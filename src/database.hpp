#pragma once

#include <string>
#include <fstream>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>

#define LOG_SPACES(count) for(int i = 0; i < count; ++i) std::cout << " ";

class Database
{
    public:
    Database(std::string name, std::string remoteuri);

    const std::string get_remote_hash();
    const std::string get_local_hash();
    const short refresh();

    const std::string db_name;
    const std::string local_path;
    const std::string remote_uri;
    const std::string remote_hash_uri;

    private:
    const std::string hash_sha256(std::string input);
    const bool local_exists();
    const bool create_local();
    const int digit_count(double num);

    std::string format_module(double bytes);
    void set_column(int col);
    void print_base_progress_bar(char progresschar, int percent);
    bool progress_callback(size_t downloadTotal, size_t downloadNow, size_t uploadTotal, size_t uploadNow);

    int progress_start;
    int progress_len;

    std::chrono::high_resolution_clock::time_point start_time;
};