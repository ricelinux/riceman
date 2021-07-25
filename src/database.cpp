#include "database.hpp"
#include "constants.hpp"

Database::Database(std::string localpath, std::string remoteuri)
: local_path{localpath}, remote_uri{remoteuri}
{}

size_t Database::write_functor(cURLpp::Easy *handle, std::fstream *stream, char *ptr, size_t size, size_t nmemb)
{
    size_t realsize = size * nmemb;

    stream->write(ptr, realsize);

    return realsize;
}

const std::string Database::get_remote_hash()
{
    curlpp::Easy request;

    std::fstream stream{local_path, std::fstream::out};

    using namespace std::placeholders;
    curlpp::options::WriteFunction *write_func = new curlpp::options::WriteFunction(std::bind(&Database::write_functor, this, &request, &stream, _1, _2, _3));

    request.setOpt(write_func);
    request.setOpt(new curlpp::options::Url(REMOTE_RICE_DB));
    
    request.perform();
}