#include "operation.hpp"

OperationHandler::OperationHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils &util)
: argparser{parser}, config{conf}, utils{util}
{}