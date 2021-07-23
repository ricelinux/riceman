#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>

class Database
{
    public:
    
    Database(std::string local_path, std::string remote_uri);

    private:

    const std::string local_path;
    const std::string remote_uri;
};

#endif // DATABASE_HPP