#include "rice.hpp"
#include "progressbar.hpp"
#include "constants.hpp"
#include "utils.hpp"

#include <fmt/format.h>
#include <cpr/cpr.h>
#include "git2.h"

Rice::Rice(std::string name, std::string id, std::string description, std::string version, std::string window_manager, std::vector<Dependency> dependencies, bool installed)
: name{name}, id{id}, description{description}, version{version}, window_manager{window_manager}, dependencies{dependencies}, installed{installed}, file_path{fmt::format("{}/{}.toml", LOCAL_TOML_DIR, id)}
{}

void Rice::install()
{
    /* Download rice's toml */
    using namespace std::placeholders;

    ProgressBar progress_bar{fmt::format("{}-{}", name, version), 0.4};
    cpr::Response r = cpr::Get(cpr::Url{fmt::format("{}/{}.toml", REMOTE_RICES_URI, id)}, cpr::ProgressCallback{std::bind(&ProgressBar::progress_callback_download, &progress_bar, _1, _2, _3, _4)});
    progress_bar.done();

    if (!Utils::write_file_content(file_path, r.text)) throw std::runtime_error{fmt::format("unable to write to '{}'", file_path)};

}

void Rice::clone_progress(const git_transfer_progress *stats, void *payload)
{
    
}