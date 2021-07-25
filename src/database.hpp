#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <fstream>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Options.hpp>

class Database
{
    public:
    Database(std::string localpath, std::string remoteuri);

    const std::string get_remote_hash();
    const std::string get_local_hash();

    const std::string local_path;
    const std::string remote_uri;

    private:

    size_t write_functor(cURLpp::Easy *handle, std::fstream *stream, char *ptr, size_t size, size_t nmemb);
    
};

#endif // DATABASE_HPP