#ifndef DATABASE_COLLECTION_HPP
#define DATABASE_COLLECTION_HPP

#include "database.hpp"

#include <vector>

class DatabaseCollection
{
    public:
    DatabaseCollection();

    Database& get(std::string &file_name);
    Database& get(int &index);
    Database& add(Database &db);
    Database& add(std::string local_path, std::string remote_uri);

    std::vector<Database> db_list;


};

#endif // DATABASE_COLLECTION_HPP