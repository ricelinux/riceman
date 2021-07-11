#include "constants.hpp"
#include "config.hpp"
#include "util.hpp"

#include <fmt/format.h>
#include <iostream>
#include <vector>
#include <getopt.h>
#include <unistd.h>

using std::cout, std::endl;

std::vector<std::string> targets;

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
            if (strcmp("never", optarg) == 0)       config->color = COLOR_DISABLED;
            else if (strcmp("auto", optarg) == 0)   config->color = isatty(fileno(stdout)) ? COLOR_ENABLED : COLOR_DISABLED;
            else if (strcmp("always", optarg) == 0) config->color = COLOR_ENABLED;
            else {
                log(LOG_ERROR, fmt::format("invalid argument '{}' for --color", optarg));
                return false;
            }

            if (config->color == COLOR_ENABLED)
            {
                config->colors.colon    = BOLDBLUE "::" BOLD " ";
                config->colors.title    = BOLD;
                config->colors.repo     = BOLDMAGENTA;
                config->colors.version  = BOLDGREEN;
                config->colors.groups   = BOLDBLUE;
                config->colors.meta     = BOLDCYAN;
                config->colors.warn     = BOLDYELLOW;
                config->colors.err      = BOLDRED;
                config->colors.faint    = GREY46;
                config->colors.nocolor  = NOCOLOR;
            } 
            else
            {
                config->colors.colon    = ":: ";
                config->colors.title    = "";
                config->colors.repo     = "";
                config->colors.version  = "";
                config->colors.groups   = "";
                config->colors.meta     = "";
                config->colors.warn     = "";
                config->colors.err      = "";
                config->colors.faint    = "";
                config->colors.nocolor  = "";
            }
            break;
        
        default:
            return false;
    };
    return true;
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
            ++config->sync.refresh;
            config->needs_root = true;
            break;
        case OP_UPGRADES:
        case 'u':
            config->sync.upgrade = true;
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

    while(optind < argc)
    {
        targets.push_back(strdup(argv[optind]));
        optind++;
    }

    // TODO: Verify arguments

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

    if (getuid() > 0 && config->needs_root)
    {
        log(LOG_ERROR, "this operation requires root permissions.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }
}