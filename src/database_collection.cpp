#include "database_collection.hpp"
#include "utils.hpp"
#include "constants.hpp"

DatabaseCollection::DatabaseCollection() {
    Utils::create_directory(LOCAL_BASE_DIR);
    Utils::create_directory(LOCAL_DB_DIR);
    Utils::create_directory(LOCAL_CONFIG_DIR);
    Utils::create_directory(LOCAL_RICES_DIR);
    Utils::create_directory(AUR_INSTALL_DIR);
};

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
    return add(*(new Database{name, remote_uri}));
}

Rice& DatabaseCollection::get_rice(std::string name)
{
    for (int i = 0; i < db_list.size(); ++i) {
        try {
            return db_list[i].get_rice(name);
        } catch (std::runtime_error err) {/* Can't find rice in current database */};
    }

    throw std::runtime_error{""}; /* No need to waste processing on formatting an error message since it won't be displayed */
}