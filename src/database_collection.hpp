#pragma once

#include "database.hpp"

#include <vector>

class DatabaseCollection
{
    public:
    DatabaseCollection();

    Database& get(std::string &file_name);
    Database& get(int &index);
    Database& add(Database &db);
    Database& add(std::string name, std::string remote_uri);

    std::vector<Database> db_list;


};