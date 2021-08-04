#include "utils.hpp"

#include <fstream>
#include <filesystem>
#include <sstream>
#include <iostream>
#include <csignal>

#include <cpr/cpr.h>
#include <fmt/format.h>

namespace fs = std::filesystem;

using CryptoPP::byte, CryptoPP::SHA256;

Utils::Utils(RicemanConfig &conf)
    : config{conf} {};

/** Get the name of the file or folder the path points to
 * @param path Path to a file or folder
 * 
 * @return The name of the file or folder that the path points to
 */
const std::string Utils::basename(const std::string &path)
{
    std::string::size_type namepos = path.rfind('/');
	
	/* If rfind returned an error */
	if (namepos != std::string::npos) ++namepos;
	else namepos = 0;

	return path.substr(namepos);
}

/** Check if config allows for certain level of logging and log accordingly
 * @param level Log level defined by enum in constants.hpp
 * @param message The string to log
 * 
 * @return A boolean, true if logged, false if not logged
 */
const bool Utils::log(const int level, const std::string &message, const bool line_break)
{
	if ((config.loglevel & level) == 0) return false;
	
	switch(level)
	{
		case LOG_FATAL:
		case LOG_ERROR:
			std::cout << config.colors.err << "error: " << config.colors.nocolor; 
			break;
		case LOG_WARNING:
			std::cout << config.colors.warn << "warning: " << config.colors.nocolor; 
			break;
		case LOG_DEBUG:
			std::cout << "debug: ";
			break;
        case LOG_ALL:
			std::cout << config.colors.nocolor;
            /* Nothing done here */
            break;
        default:
            return false;
	}

	std::cout << message;
	if (line_break) std::cout << std::endl;
	

	if (level == LOG_FATAL) {
		show_cursor(true);
		handle_signals(false);
		exit(EXIT_FAILURE);
	}

	return true;
}

const bool Utils::log(const int level, const int &message, const bool line_break)
{
	return log(level, std::to_string(message), line_break);
}

void Utils::colon_log(const std::string &message)
{
	std::cout << config.colors.colon << message << config.colors.nocolor << std::endl;
}

void Utils::rice_log(const std::vector<Rice> &rices)
{
	std::cout << std::endl << config.colors.title << "Rices (" << rices.size() << ") " << config.colors.nocolor;

	for(int i = 0; i < rices.size(); ++i) {
		std::cout << rices[i].name << config.colors.faint << "-" << rices[i].version << config.colors.nocolor << "  ";
	}

	std::cout << std::endl << std::endl;
}

void Utils::show_cursor(const bool status)
{
	if (status) std::cout << "\033[?25h";
	else std::cout << "\033[?25l";
}

void Utils::handle_signals(const bool status)
{
	if (status) {
		signal(SIGINT, Utils::handle_sigint);
	} else {
		signal(SIGINT, NULL);
	}
	
}

void Utils::handle_sigint(int signum)
{
	show_cursor(true);
	std::cout << "error: interrupt received" << std::endl;
	exit(signum);
}

std::string Utils::get_file_content(const std::string &path)
{
	std::ifstream file{path};
    if (file.is_open()) {
        std::ostringstream ss;
        ss << file.rdbuf();
		file.close();
        return ss.str();
    } else throw std::runtime_error{fmt::format("unable to read '{}'", path)};
}

const bool Utils::write_file_content(const std::string &path, const std::string &content)
{
	std::ofstream stream;
    stream.open(path, std::fstream::out);
    if (!stream.is_open()) return false;
    stream.write(content.c_str(), content.length());
    stream.close();
	return true;
}

void Utils::create_directory(const std::string &path)
{
	if (fs::exists(path) && !fs::is_directory(path)) fs::remove(path);
    if (!fs::exists(path)) {
        fs::create_directory(path);
        fs::permissions(path, fs::perms::owner_all | fs::perms::group_exec | fs::perms::group_read | fs::perms::others_read | fs::perms::others_exec);
    }
}

const std::string Utils::get_uri_content(const std::string &uri)
{
	cpr::Response r = cpr::Get(cpr::Url(uri));
    if (r.error) throw std::runtime_error{r.error.message};
    r.text.erase(std::remove(r.text.begin(), r.text.end(), '\n'), r.text.end());
    return r.text;
}

const std::string Utils::hash_sha256(const std::string &content)
{
	SHA256 hash;
    byte digest[SHA256::DIGESTSIZE];

    hash.CalculateDigest(digest, (byte *)content.c_str(), content.length());
    
    CryptoPP::HexEncoder encoder;
    std::string output;
    encoder.Attach(new CryptoPP::StringSink(output));
    encoder.Put(digest, sizeof(digest));
    encoder.MessageEnd();

    return output;
}

const std::string Utils::hash_file(const std::string &path)
{
	return hash_sha256(get_file_content(path));
}
