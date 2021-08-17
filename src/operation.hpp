#pragma once

#include "config.hpp"
#include "utils.hpp"
#include "database_collection.hpp"

#include <argparse/argparse.hpp>

class OperationHandler
{
    public:

    OperationHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils &util, DatabaseCollection &databases);

    virtual void run() = 0;

    protected:

    argparse::ArgumentParser &argparser;
    RicemanConfig &config;
    Utils &utils;
    DatabaseCollection &databases;
};
