#pragma once

#include "database_rice.hpp"

class InstalledRice : public DatabaseRice {
    
    public:

    InstalledRice(std::string name);
    InstalledRice(DatabaseRice &rice);
};