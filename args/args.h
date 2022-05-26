#pragma once

#include "containers/string.h"
#include "containers/darray.h"

typedef enum
{
    COMMAND_NONE,
    COMMAND_SHOW,
    COMMAND_FIND,
    COMMAND_TAGS,
    COMMAND_ADD,
    COMMAND_REMOVE,
    COMMAND_EDIT,
    COMMAND_INIT,
    COMMAND_DELETE,
} Command_Code;

typedef enum
{
    SCOPE_LOCAL,
    SCOPE_GLOBAL,
} Scope;

typedef struct
{
    int error;
    Command_Code command;
    DArray(String) options;
    int show_tags;
    int show_help;
    Scope scope;
} CL_Args;

CL_Args parse_command_line_args(int argc, char* argv[]);
void free_cl_args(CL_Args* args);