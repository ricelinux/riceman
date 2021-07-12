#include "sync.hpp"
#include "config.hpp"
#include "constants.hpp"
#include "util.hpp"
#include "progressbar.hpp"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <fmt/format.h>
#include <errno.h>

FILE *db_file;

size_t sync_write_file(char* ptr, size_t size, size_t nmemb)
{
    fwrite(ptr, size, nmemb, db_file);
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
        db_file = fopen(RICE_DB_PATH, "w");
        if(!db_file) {
            log(LOG_ERROR, strerror(errno));
            exit(EXIT_FAILURE);
        }

        ProgressBar progressbar{"rices", '#'};

        curlpp::Cleanup curl_cleanup;
        curlpp::Easy req;

        using namespace std::placeholders;

        curlpp::types::ProgressFunctionFunctor progress_functor = std::bind(&ProgressBar::update_progress, &progressbar, _1, _2, _3, _4);
        

        curlpp::options::WriteFunction *write_func = new curlpp::options::WriteFunction{&sync_write_file};
        curlpp::options::ProgressFunction *progress_func = new curlpp::options::ProgressFunction{progress_functor};
        
        req.setOpt(write_func);
        req.setOpt(progress_func);
        
        req.setOpt(new curlpp::options::Url(RICE_DB));
        req.setOpt(new curlpp::options::NoProgress(0));
        //req.setOpt(new curlpp::options::Verbose(true));

        req.perform();

        fclose(db_file);
    }
    catch ( curlpp::LogicError & e )
	{
		log(LOG_ERROR, fmt::format("format error - {}", e.what()));
	}
	catch ( curlpp::RuntimeError & e )
	{
        log(LOG_ERROR, fmt::format("runtime error - {}", e.what()));
    }

}

bool riceman_sync(std::vector<std::string> targets)
{
    if (config->sync.refresh) {
        colon_log("Synchronize rice database...");
        sync_refresh();
    }
    
    if (config->sync.upgrade) {
        
    }
}