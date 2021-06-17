#include <stdio.h>

#include "filestuff/filestuff.h"
#include "todos/todo.h"
#include "args/args.h"
#include "todos/help_strings.h"

#define STRING_IMPL
#include "containers/string.h"

#define DARRAY_IMPL
#include "containers/darray.h"

#define DICTIONARY_IMPL
#include "containers/dictionary.h"

// #define DEBUG

#ifdef DEBUG
int main()
{
    char* argv[] = {
        "todo-test",
        "-local",
        "init",
    };
    int argc = 3;
#else
int main(int argc, char* argv[])
{
#endif
    // All the cleanup will be handled by the OS

    CL_Args args = parse_command_line_args(argc, argv);

    if (args.error)
        return 1;

    if (args.command == COMMAND_NONE)
    {
        printf(help_strings[COMMAND_NONE], argv[0], argv[0], argv[0], argv[0], argv[0]);
        free_cl_args(&args);
        return 0;
    }

    if (args.show_help)
    {
        printf(help_strings[args.command], argv[0]);
        return 0;
    }

    String filepath = (args.scope == SCOPE_LOCAL) ? ".todos" : "C:/todos/.todos";
    String contents = load_file(filepath);

    if (contents == NULL && args.command != COMMAND_INIT)
    {
        printf(
            "No todo list associated with this directory.\n"
            "To initialize a todo list in this directory, call:\n"
            "    %s init\n", argv[0]
        );

        return 1;
    }

    File_Data data = todos_file_make();
    todos_file_load(contents, &data);

    switch (args.command)
    {
        case COMMAND_SHOW:
        {
            int num_options = da_size(args.options);
            if (num_options == 0 || string_cmp(args.options[0], "*"))
            {
                int size = da_size(data.todos);

                if (size == 0)
                {
                    printf("No todos found\n");
                    return 1;
                }

                for (int i = 0; i < size; i++)
                {
                    printf("%d. %s", i + 1, data.todos[i].task);
                    if (args.show_tags)
                    {
                        printf(" :");
                        da_foreach(String, tag, data.todos[i].tags)
                            printf(" %s", *tag);
                    }

                    printf("\n");
                }
            }
            else
            {
                int num_todos = da_size(data.todos);
                unsigned char* to_be_displayed = calloc(num_todos, sizeof(unsigned char));

                da_foreach(String, option, args.options)
                {
                    if ((*option)[0] == '#')
                    {
                        Dict_Bkt(DArray(int)) bkt = dict_find(data.tag_dict, (*option));
                        if (bkt == dict_end(data.tag_dict))
                        {
                            printf("%s tag not found\n", (*option));
                            return 1;
                        }

                        da_foreach(int, it, bkt->value)
                            to_be_displayed[*it] = 1;
                    }
                    else
                    {
                        int index = atoi(*option);

                        // Not a valid number
                        if (index == 0)
                        {
                            printf("Expected a tag or a number, got: '%s'\n", *option);
                            continue;
                        }

                        if (index > da_size(data.todos))
                        {
                            printf("%d is an invalid index\n", index);
                            continue;
                        }

                        to_be_displayed[index - 1] = 1;
                    }
                }

                for (int i = 0; i < num_todos; i++)
                {
                    if (to_be_displayed[i])
                    {
                        printf("%d. %s", i + 1, data.todos[i].task);
                        if (args.show_tags)
                        {
                            printf(" :");
                            da_foreach(String, tag, data.todos[i].tags)
                                printf(" %s", *tag);
                        }
                        printf("\n");
                    }
                }

                free(to_be_displayed);
            }
        } break;

        case COMMAND_FIND:
        {
            int num_options = da_size(args.options);
            if (num_options == 0)
            {
                printf("No term provided to find\n");
                return 1;
            }

            int num_todos = da_size(data.todos);
            int count = 0;
            for (int i = 0; i < num_todos; i++)
            {
                da_foreach(String, search_string, args.options)
                {
                    char* found = strstr(data.todos[i].task, (*search_string));
                    if (found)
                    {
                        printf("%d. %s\n", i + 1, data.todos[i].task);
                        count++;
                        break;
                    }
                }
            }

            char s = (count == 1) ? '\0' : 's';
            printf("Found %d todo%c\n", count, s);
        } break;

        case COMMAND_TAGS:
        {
            dict_foreach(DArray(int), bkt, data.tag_dict)
            {
                if (bkt->key)
                    printf("%s (%d)\n", bkt->key, da_size(bkt->value));
            }

            int num_tags = dict_filled(data.tag_dict);
            char* new_line = (num_tags > 0) ? "\n" : "";
            printf("%sFound %d tag(s)\n", new_line, num_tags);
        } break;

        case COMMAND_ADD:
        {
            int num_options = da_size(args.options);
            if (num_options == 0)
            {
                printf("No task provided to add\n");
                return 1;
            }

            Todo todo = todo_make(string_make(args.options[0]));
            for (int i = 1; i < num_options; i++)
            {
                // Ignore invalid tags
                if (args.options[i][0] != '#')
                    continue;

                da_push_back(todo.tags, string_make(args.options[i]));
            }

            todos_add_todo(&data, todo);
            String new_content = todos_generate_content(data);
            write_file(filepath, new_content);

            printf("Added todo: %s", todo.task);
        } break;

        case COMMAND_REMOVE:
        {
            int num_options = da_size(args.options);
            if (num_options == 0)
            {
                printf("Provide index of todo or tags to be removed\n");
                return 1;
            }

            if (string_cmp(args.options[0], "*"))
            {
                write_file(filepath, "");
                return 0;
            }

            int num_todos = da_size(data.todos);
            unsigned char* mark_for_removing = calloc(num_todos, sizeof(unsigned char));

            da_foreach(String, option, args.options)
            {
                if ((*option)[0] == '#')
                {
                    // Remove by tag
                    Dict_Bkt(DArray(int)) bkt = dict_find(data.tag_dict, (*option));
                    if (bkt == dict_end(data.tag_dict))
                    {
                        printf("Tag not found: %s\n", *option);
                        continue;
                    }

                    for (int i = da_size(bkt->value) - 1; i >= 0; i--)
                    {
                        int index = bkt->value[i];
                        mark_for_removing[index] = 1;
                    }
                }
                else
                {
                    // Remove by index
                    int index = atoi(*option);

                    // Not a valid number
                    if (index == 0)
                    {
                        printf("Expected a tag or a number, got: '%s'\n", *option);
                        continue;
                    }

                    if (index > da_size(data.todos))
                    {
                        printf("%d is an invalid index\n", index);
                        continue;
                    }

                    mark_for_removing[index - 1] = 1;
                }
            }

            // Displaying names in proper order
            for (int i = 0; i < num_todos; i++)
            {
                if (mark_for_removing[i])
                    printf("Removed todo: %s\n", data.todos[i].task);
            }

            // Deleting in reverse order since the other way
            // round can cause errors due to change in order in todos array
            for (int index = num_todos - 1; index >= 0; index--)
            {
                if (mark_for_removing[index])
                    da_erase_at(data.todos, index);
            }

            free(mark_for_removing);

            String new_content = todos_generate_content(data);
            write_file(filepath, new_content);
        } break;

        case COMMAND_EDIT:
        {
            int num_options = da_size(args.options);
            if (num_options == 0)
            {
                printf("Expected index of todo to be edited\n");
                return 1;
            }

            int index = atoi(args.options[0]);

            // Not a valid number
            if (index == 0)
            {
                printf("Expected a tag or a number, got: '%s'\n", args.options[0]);
                return 1;
            }

            if (index > da_size(data.todos))
            {
                printf("%d is an invalid index\n", index);
                return 1;
            }

            Todo new_todo = todo_make(args.options[1]);

            for (int i = 2; i < num_options; i++)
            {
                // Ignore invalid tags
                if (args.options[i][0] != '#')
                    continue;

                da_push_back(new_todo.tags, string_make(args.options[i]));
            }

            data.todos[index - 1] = new_todo;

            String new_content = todos_generate_content(data);
            write_file(filepath, new_content);

            printf("Todo updated\n");
        } break;

        case COMMAND_INIT:
        {
            // Create an empty list file locally
            write_file(".todos", "");
            printf("Todo list initilazed for current directory\n");
        } break;

        case COMMAND_DELETE:
        {
            // Delete local list file
            if (!remove_file(".todos"))
            {
                printf("Couldn't delete todo-list.txt in current directory\n");
                return 1;
            }

            printf("Deleted todo list for current directory\n");
        } break;
    }
}