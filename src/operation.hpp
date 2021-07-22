#ifndef OPERATION_HPP
#define OPERATION_HPP

#include "config.hpp"
#include "utils.hpp"

#include "argparse/argparse.hpp"

class OperationHandler
{
    public:

    OperationHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils &util);

    virtual bool run() = 0;

    protected:

    argparse::ArgumentParser &argparser;
    RicemanConfig &config;
    Utils &utils;
};

#endif