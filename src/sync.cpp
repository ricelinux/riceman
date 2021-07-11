#include "sync.hpp"
#include "config.hpp"
#include "constants.hpp"
#include "util.hpp"

#include <iostream>

bool sync_refresh()
{
    
}

bool riceman_sync(std::vector<std::string> targets)
{
    if (config->sync.refresh)
    {
        colon_log("Synchronize rice database...");
        sync_refresh();
    }
    
    if (config->sync.upgrade)
    {
        
    }
}