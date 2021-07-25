#include "operation.hpp"

OperationHandler::OperationHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils &util, DatabaseCollection &databases)
: argparser{parser}, config{conf}, utils{util}, databases{databases}
{}