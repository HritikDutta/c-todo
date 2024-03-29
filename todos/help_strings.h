#pragma once

// TODO: Add help strings for move command.

const char* help_strings[] = {

// NONE
"A todo-system for the command line.\n"
"    usage: %s [-help] [-local] [-global] <command> [<args>]\n"
"\n"
"List of commands used in c-todo:\n"
"\n"
"read commands\n"
"    show    Show todos (all or with given index/tag)\n"
"    find    Find for todos that contain given subtring\n"
"    tags    List all tags\n"
"\n"
"write commands\n"
"    add     Add todo and tags\n"
"    remove  Remove todo with given index/tag\n"
"    edit    Edit todo with given index\n"
"    move    Move todo to another index\n"
"\n"
"local commands\n"
"    init    Initialize a local todo-list for current directory\n"
"    delete  Delete local todo-list\n"
"\n"
"For more information about specific commands, use -help\n"
"along with the command. Like below:\n"
"\n"
"    $ %s show -help OR $ %s -help show\n"
"\n"
"By default the scope is considered local. To manually set the scope of the command\n"
"to local or global, use -local or -global along with the command. Like below:\n"
"\n"
"    $ %s show -local OR $ %s -global show\n"
,

// SHOW
"Show todos (all or with given index/tag).\n"
"    usage: %s show [-with-tags] [<index>...] [<tag>...]\n"
"\n"
"options\n"
"    <index>...  Indices of todos to be shown (* displays all indices)\n"
"    <tag>...    All todos with these tags will be shown (tags start with an '#' and contain no whitespaces)\n"
"    -with-tags  Show tags linked with displayed todos\n"
,

// FIND
"Find todos that contain given subtring.\n"
"    usage: %s find [-with-tags] <arg>\n"
"\n"
"options\n"
"    <arg>       Substring to search in all todos\n"
"    -with-tags  Show tags linked with displayed todos\n"
,

// TAGS
"List all tags.\n"
"    usage: %s tags\n"
"\n"
"Any additional options will be ignored.\n"
,

// ADD
"Add todo and tags.\n"
"    usage: %s add <task> [<tag>...]\n"
"\n"
"options\n"
"    <task>   Task for new todo\n"
"    <tag>... Tags associated with new todo (tags start with an '#' and contain no whitespaces)\n"
,

// REMOVE
"Remove todo with given index/tag.\n"
"    usage: %s remove [<index>...] [<tags>...]\n"
"\n"
"options\n"
"    <index>... Indices of todos to be removed (* removes all indices)\n"
"    <tag>...   All todos with these tags will be removed (tags start with an '#' and contain no whitespaces)\n"
,

// EDIT
"Edit todo with given index.\n"
"    usage: %s edit <index> <task> [<tags>...]\n"
"\n"
"options\n"
"    <index>  Index of todo to be edited\n"
"    <task>   New task for todo\n"
"    <tag>... New tags associated with selected todo (tags start with an '#' and contain no whitespaces)\n"
,

// MOVE
"Move a todo from one index to another.\n"
"    usage: %s move <source-index> <destination-index>\n"
"\n"
"options\n"
"    <source-index>       Index of todo to be moved\n"
"    <destination-index>  Index to move the todo into\n"
,

// INIT
"Initialize a local todo-list for current directory.\n"
"    usage: %s init\n"
,

// DELETE
"Delete local todo-list.\n"
"    usage: %s delete\n"
,

};