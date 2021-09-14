// #pragma once

// #include "option.hpp"
// #include "operation.hpp"
// #include "constants.hpp"
// #include "database_rice.hpp"

// class RemoveHandler: public OperationHandler
// {
//     public:

//     RemoveHandler(argparse::ArgumentParser &parser, RicemanConfig &conf, Utils &util, DatabaseCollection &databases);
//     void run();
//     void remove_rices();

//     static const int op_modifiers_s = 4;
//     static const struct option<int> op_modifiers[op_modifiers_s];

//     std::vector<std::string> targets;
//     std::vector<std::string> incorrect_rice_names;
//     std::vector<DatabaseRice> rices;

//     /* Operation Modifiers */
//     bool remove_git;
//     bool remove_toml;
//     bool remove_desktop;
// };