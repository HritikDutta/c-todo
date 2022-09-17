#include "args.h"

#include <stdio.h>

#include "containers/hd_assert.h"
#include "containers/string.h"
#include "containers/darray.h"

CL_Args parse_command_line_args(int argc, char* argv[])
{
    CL_Args args = { 0 };
    da_make(args.options);

    if (argc == 1)
        return args;

    int found_commmand = 0;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if (string_cmp(argv[i], "-help"))
            {
                args.show_help = 1;
                continue;
            }

            // Just in case anyone wants to be explicit
            // By default, the scope is local
            if (string_cmp(argv[i], "-local"))
            {
                if (args.scope != SCOPE_LOCAL)
                {
                    printf("Trying to set scope twice\n");
                    args.error = 1;
                    break;
                }

                continue;
            }
            
            if (string_cmp(argv[i], "-with-tags"))
            {
                args.show_tags = 1;
                continue;
            }

            if (string_cmp(argv[i], "-global"))
            {
                args.scope = SCOPE_GLOBAL;
                continue;
            }

            printf("Flag '%s' not recognized\n", argv[i]);
            args.error = 1;
            break;
        }

        if (!found_commmand)
        {
            if (string_cmp(argv[i], "show"))
            {
                args.command = COMMAND_SHOW;
                found_commmand = 1;
                continue;
            }

            if (string_cmp(argv[i], "find"))
            {
                args.command = COMMAND_FIND;
                found_commmand = 1;
                continue;
            }

            if (string_cmp(argv[i], "tags"))
            {
                args.command = COMMAND_TAGS;
                found_commmand = 1;
                continue;
            }

            if (string_cmp(argv[i], "add"))
            {
                args.command = COMMAND_ADD;
                found_commmand = 1;
                continue;
            }

            if (string_cmp(argv[i], "remove"))
            {
                args.command = COMMAND_REMOVE;
                found_commmand = 1;
                continue;
            }

            if (string_cmp(argv[i], "edit"))
            {
                args.command = COMMAND_EDIT;
                found_commmand = 1;
                continue;
            }

            if (string_cmp(argv[i], "move"))
            {
                args.command = COMMAND_MOVE;
                found_commmand = 1;
                continue;
            }

            if (string_cmp(argv[i], "init"))
            {
                args.command = COMMAND_INIT;
                found_commmand = 1;
                continue;
            }

            if (string_cmp(argv[i], "delete"))
            {
                args.command = COMMAND_DELETE;
                found_commmand = 1;
                continue;
            }

            printf("Command '%s' not recognized\n", argv[i]);
            args.error = 1;
            break;
        }
        else
        {
            String arg = string_make(argv[i]);
            da_push_back(args.options, arg);
        }
    }

    return args;
}

void free_cl_args(CL_Args* args)
{
    da_foreach(String, arg, args->options)
    {
        if (*arg)
            string_free(arg);
    }

    da_free(args->options);
}