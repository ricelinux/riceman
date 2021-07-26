#include "database_collection.hpp"

DatabaseCollection::DatabaseCollection() {};

Database& DatabaseCollection::get(std::string &file_name)
{
    for(int i = 0; i < db_list.size(); i++) {
        if (db_list[i].local_path.compare(file_name) == 0) return db_list[i];
    }
    throw std::runtime_error{"database not found"};
}

Database& DatabaseCollection::get(int &index)
{
    return db_list[index];
}

Database& DatabaseCollection::add(Database &db)
{
    db_list.push_back(db);
    return db;
}

Database& DatabaseCollection::add(std::string name, std::string remote_uri)
{
    Database *db = new Database{name, remote_uri};
    return add(*db);
}