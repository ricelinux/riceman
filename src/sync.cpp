#include "sync.hpp"
#include "config.hpp"
#include "constants.hpp"
#include "util.hpp"
#include "progressbar.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include <sys/stat.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>
#include <fmt/format.h>
#include <errno.h>

namespace fs = std::filesystem;
using CryptoPP::byte;

FILE *db_file;

size_t sync_write_file(char* ptr, size_t size, size_t nmemb)
{
    fwrite(ptr, size, nmemb, db_file);
    return 0;
}

std::string sync_hash(std::string &path)
{
    if (fs::exists(path) && !fs::is_directory(path)) {
        CryptoPP::SHA256 db_hash;
        std::string db_digest;
        std::ifstream ifs{path};
        std::string db_content{(std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())};

        db_hash.Update((const byte*)db_content.data(), db_content.size());
        db_digest.resize(db_hash.DigestSize());
        db_hash.Final((byte*)db_digest[0]);

        log(LOG_WARNING, db_digest);

    } else {
        return NULL;
    }
}

bool sync_refresh()
{
    switch(dir_exists(RICE_LIB_PATH))
    {
        case -1:
            log(LOG_ERROR, fmt::format("{} is not a directory. Please move this file.", RICE_LIB_PATH));
            return false;
        case -2:
            mkdir(RICE_LIB_PATH, S_IRWXU);
            break;
    }

    try 
    {
        db_file = fopen(RICE_DB_PATH, "w+");
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
        
        log(LOG_WARNING, RICE_DB_PATH);

        req.setOpt(new curlpp::options::Url(RICE_REPO_DB));
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

    return true;
}

bool riceman_sync(std::vector<std::string> targets)
{
    if (config->sync.refresh) {
        colon_log("Synchronize rice database...");
        sync_refresh();
    }
    
    if (config->sync.upgrade) {
        
    }

    return true;
}