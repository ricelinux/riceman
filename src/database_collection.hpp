#ifndef DATABASE_COLLECTION
#define DATABASE_COLLECTION

#include "database.hpp"

class DatabaseCollection
{
    private:

    Database databases[];

    public:

    Database& get(std::string file_name);
    Database& add(Database &db);
    Database& add(std::string local_path, std::string remote_uri);

};

#endif