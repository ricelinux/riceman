#include "database.hpp"
#include "constants.hpp"

#include <fstream>
#include <sstream>
#include <filesystem>

#include <cpr/cpr.h>
#include <fmt/format.h>

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>

using CryptoPP::byte, CryptoPP::SHA256;

namespace fs = std::filesystem;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

Database::Database(std::string name, std::string remoteuri)
: db_name{name}, local_path{fmt::format("{}/{}{}", LOCAL_RICE_BASE_URI, name, ".db")}, remote_uri{remoteuri}, remote_hash_uri{fmt::format("{}.sha256sum", remoteuri)}
{}

const bool Database::create_local()
{
    if (fs::exists(LOCAL_RICE_BASE_URI) && !fs::is_directory(LOCAL_RICE_BASE_URI)) fs::remove(LOCAL_RICE_BASE_URI);
    if (!fs::exists(LOCAL_RICE_BASE_URI)) {
        fs::create_directory(LOCAL_RICE_BASE_URI);
        fs::permissions(LOCAL_RICE_BASE_URI, fs::perms::owner_all | fs::perms::group_exec | fs::perms::group_read | fs::perms::others_read | fs::perms::others_exec);
    }

    std::ofstream stream;

    stream.open(local_path, std::fstream::out);
    if (!stream.is_open()) return false;
    stream.write("", 0);

    fs::permissions(local_path, fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read | fs::perms::others_read);
    
    return true;
}

const bool Database::local_exists()
{
    return std::filesystem::exists(local_path);
}

/** Downloads a fresh copy of the database
 * 
 * @returns -1 if request fails, -2 if write fails, 1 if successful
 */
const short Database::refresh()
{
    using namespace std::placeholders;

    print_base_progress_bar('-', 0);
    start_time = Clock::now();
    cpr::Response r = cpr::Get(cpr::Url{remote_uri},
                      cpr::ProgressCallback(std::bind(&Database::progress_callback, this, _1, _2, _3, _4)));
    std::cout << std::endl;

    if (r.error) return -1;

    std::ofstream stream;
    stream.open(local_path, std::fstream::out);
    if (!stream.is_open()) return -2;

    stream.write(r.text.c_str(), r.text.length());
    return 1;
}

const std::string Database::hash_sha256(std::string input)
{
    SHA256 hash;
    byte digest[SHA256::DIGESTSIZE];

    hash.CalculateDigest(digest, (byte *)input.c_str(), input.length());
    
    CryptoPP::HexEncoder encoder;
    std::string output;
    encoder.Attach(new CryptoPP::StringSink(output));
    encoder.Put(digest, sizeof(digest));
    encoder.MessageEnd();

    return output;
}

const std::string Database::get_remote_hash()
{    
    cpr::Response r = cpr::Get(cpr::Url(remote_hash_uri));
    if (r.error) throw std::runtime_error{r.error.message};
    r.text.erase(std::remove(r.text.begin(), r.text.end(), '\n'), r.text.end());
    return r.text;
}

const std::string Database::get_local_hash()
{
    if (!local_exists() && !create_local()) throw std::runtime_error{fmt::format("unable to access {}", local_path)};
    std::ifstream file{local_path};
    std::ostringstream ss;

    ss << file.rdbuf();

    return hash_sha256(ss.str());
}

void Database::set_column(int col)
{
    std::cout << "\r\033[" << col << "C";
}

void Database::print_base_progress_bar(char progresschar, int percent)
{
    struct winsize window;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);

    int bar_width = floor(window.ws_col * 0.4);
    std::cout << "\r " << db_name;
    set_column(window.ws_col - bar_width);
    std::cout << "[";
    
    for(int i = 0; i < bar_width - 7; ++i) std::cout << progresschar;
    std::cout << "] ";
    LOG_SPACES(3 - digit_count(percent))
    std::cout << percent << "%";
    progress_start = (window.ws_col - bar_width) + 1;
    progress_len = bar_width - 7;
}

bool Database::progress_callback(size_t dtotal, size_t dnow, size_t utotal, size_t unow)
{
    milliseconds millis = std::chrono::duration_cast<milliseconds>(Clock::now() - start_time);
    double rate = (double)dnow / millis.count();

    set_column(progress_start - 26);

    std::cout << format_module(dnow) << format_module(rate * 1000) << "/s [";

    int completed_num = floor(progress_len * ((double)dnow / (double)(dtotal == 0 ? 1 : dtotal)));

    for (int i = 0; i < progress_len; ++i) {
        if (dtotal != 0) {
            if (i <= completed_num) std::cout << "#";
            else std::cout << "-";
        } else std::cout << "-";
    }

    std::cout << "] ";

    double percentage_completed = trunc(((double)dnow / (double)(dtotal == 0 ? 1 : dtotal)) * 100);
    if (percentage_completed < 10) {
        std::cout << "  ";
    } else if (percentage_completed < 100) {
        std::cout << " ";
    }

    std::cout << percentage_completed << "%";

    return true;
}

const int Database::digit_count(double num)
{
    int digits = 0;
    while (num > 0) {
        ++digits;
        num = num / 10;
    }
    return digits == 0 ? 1 : digits;
}

std::string Database::format_module(double bytes)
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