#include "sync.hpp"
#include "config.hpp"
#include "constants.hpp"
#include "util.hpp"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <fmt/format.h>

size_t sync_write_file(char* ptr, size_t size, size_t nmemb)
{
    FILE *file = fopen(RICE_DB_PATH, "w");
    
    fwrite(ptr, size, nmemb, file);
}

double sync_dl_progress(double dltotal, double dlnow, double ultotal, double ulnow)
{
    std::cout << dlnow << "/" << dltotal << std::endl;
}

bool sync_refresh()
{   
    switch(dir_exists(RICE_LIB_PATH))
    {
        case -2:
            log(LOG_ERROR, fmt::format("{} is not a directory. Please move this file.", RICE_LIB_PATH));
            return false;
        case -1:
            mkdir(RICE_LIB_PATH, S_IRWXU);
            break;
    }



    try 
    {
        curlpp::Cleanup curl_cleanup;
        curlpp::Easy req;

        curlpp::options::WriteFunction *write_func = new curlpp::options::WriteFunction(&sync_write_file);
        req.setOpt(write_func);

        curlpp::options::ProgressFunction progress_bar{&sync_dl_progress};
        req.setOpt(new curlpp::options::Url(RICE_DB));
        req.setOpt(new curlpp::options::NoProgress(0));
        req.setOpt(progress_bar);
        //req.setOpt(new curlpp::options::Verbose(true));

        req.perform();
        
    }
    catch ( curlpp::LogicError & e )
	{
		std::cout << e.what() << std::endl;
	}
	catch ( curlpp::RuntimeError & e )
	{
		std::cout << e.what() << std::endl;
	}

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