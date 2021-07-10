#include "constants.hpp"
#include "config.hpp"
#include "util.hpp"

#include <iostream>
#include <getopt.h>

using std::cout, std::endl;

void help(int &op, std::string pname)
{
    cout << pname << endl;
}

void version()
{
    cout << "Riceman " << VERSION << endl
         << "Copyright (C) 2021 Zaedus" << endl
         << endl
         << "This program may be freely redistributed under" << endl
         << "the terms of the GNU General Public License." << endl;
}

static int parsearg_sync(int opt)
{
    switch (opt)
    {
        case OP_REFRESH:
        case 'y':
            config->sync.refresh = 1;
            break;
    };
}

static int parsearg_op(int opt, bool dryrun)
{
    switch(opt) {
        case 'S':
            if(dryrun) break;
            config->op = (config->op != OP_UNSET ? 0 : OP_SYNC); break;
        case 'R':
            if(dryrun) break;
            config->op = (config->op != OP_UNSET ? 0 : OP_REMOVE); break;
        case 'Q':
            if(dryrun) break;
            config->op = (config->op != OP_UNSET ? 0 : OP_QUERY); break;
        case 'V':
            if(dryrun) break;
            config->version = true; break;
        case 'h':
            if(dryrun) break;
            config->help = true; break;
        default:
            return 1;
    }
    return 0;
}

static int parseargs(int argc, char *argv[])
{
    int ret = 0;
    int opt;
    int option_index = 0;
    const char *optstring = "SRQVy";
    static const struct option opts[] =
    {
        { "sync",       no_argument,    0,  'S' },
        { "remove",     no_argument,    0,  'R' },
        { "query",      no_argument,    0,  'Q' },
        { "version",    no_argument,    0,  'V' },
        { "help",       no_argument,    0,  'h' },

        { "refresh",    no_argument,    0,  OP_REFRESH },
        { "upgrade",    no_argument,    0,  OP_UPGRADES },

        {0, 0, 0, 0}
    };
    
    // parse operational args
    while((opt = getopt_long(argc, argv, optstring, opts, &option_index)) != -1)
    {
		if(opt == 0) {
			continue;
		} else if(opt == '?') {
			return 1;
		}
		parsearg_op(opt, 0);
	}

    if (config->op == 0)
    {
        log(LOG_ERROR, "too many operations specified");
        return 1;
    }

    if (config->help)
    {
        help(config->op, basename(argv[0]));
        exit(EXIT_SUCCESS);
    }

    if (config->version)
    {
        version();
        exit(EXIT_SUCCESS);
    }

    // parse other args
    optind = 1;
    while((opt = getopt_long(argc, argv, optstring, opts, &option_index)) != 1)
    {
        if (opt == 0)
        {
            continue;
        } 
        else if (opt == '?')
        {
            return 1;
        }
        else if (parsearg_op(opt, true) == 0)
        {
            continue;
        }
    }

    return ret;
}

int main(int argc, char *argv[]) 
{
    config = new RicemanConfig();
    parseargs(argc, argv);
}