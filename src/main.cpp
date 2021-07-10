#include "constants.hpp"
#include "config.hpp"
#include "util.hpp"

#include <iostream>
#include <fmt/format.h>
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

bool cleanup()
{
    // TODO: show cursor, stop handling interrupts

    free(config);
}

bool parsearg_global(int opt)
{
    switch(opt)
    {
        case OP_COLOR:
            break;
    };
}

bool parsearg_remove(int opt)
{
    // TODO: handle remove opts
}

bool parsearg_query(int opt)
{
    // TODO: handle query opts
}

bool parsearg_sync(int opt)
{
    switch (opt)
    {
        case OP_REFRESH:
        case 'y':
            config->sync.refresh = 1;
            break;
        case OP_UPGRADES:
        case 'u':
            config->sync.upgrade = 1;
            break;
        default:
            return false;
            break;
    };

    return true;
}

bool parsearg_op(int opt, bool dryrun)
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
            return false;
            break;
    }
    return true;
}

bool parseargs(int argc, char *argv[])
{
    bool ret = true;
    int opt;
    int option_index = 0;
    const char *optstring = "SRQVhyu";
    static const struct option opts[] =
    {
        { "sync",       no_argument,        0,  'S' },
        { "remove",     no_argument,        0,  'R' },
        { "query",      no_argument,        0,  'Q' },
        { "version",    no_argument,        0,  'V' },
        { "help",       no_argument,        0,  'h' },

        { "refresh",    no_argument,        0,  OP_REFRESH },
        { "upgrade",    no_argument,        0,  OP_UPGRADES },

        { "color",      required_argument,  0,  OP_COLOR },

        {0, 0, 0, 0}
    };
    
    // parse operational args
    while((opt = getopt_long(argc, argv, optstring, opts, &option_index)) != -1)
    {
		if(opt == 0)
        {
			continue;
		} 
        else if(opt == '?')
        {
			return false;
		}
		parsearg_op(opt, 0);
	}

    if (config->op == 0)
    {
        log(LOG_ERROR, "too many operations specified");
        return false;
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
    while((opt = getopt_long(argc, argv, optstring, opts, &option_index)) != -1)
    {
        if (opt == 0)
        {
            continue;
        } 
        else if (opt == '?')
        {
            return false;
        }
        else if (parsearg_op(opt, true))
        {
            continue;
        }

        switch(config->op)
        {
            case OP_SYNC:
                ret = parsearg_sync(opt);
                break;
            default:
                ret = false;
                break;
        };

        // if option found, don't check if opt is global opt
        if (ret) continue;
        
        ret = parsearg_global(opt);

        // if parsearg_global fails
        if (!ret)
        {
            // detect if opt is a code or a string
            if (opt < OP_LONG_MIN) log(LOG_ERROR, fmt::format("invalid option '-{}'\n", (char)opt));
            else log(LOG_ERROR, fmt::format("invalid option '--{}'\n", std::string{opts[option_index].name}));
        }
    }

    return ret;
}

int main(int argc, char *argv[]) 
{
    bool ret;

    config = new RicemanConfig();
    ret = parseargs(argc, argv);

    if (!ret)
    {
        cleanup();
        exit(EXIT_FAILURE);
    }
}