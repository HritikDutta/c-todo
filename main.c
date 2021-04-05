#include <stdio.h>

#include "filestuff/filestuff.h"
#include "todos/todo.h"

#define STRING_IMPL
#include "containers/string.h"

#define DARRAY_IMPL
#include "containers/darray.h"

#define DICTIONARY_IMPL
#include "containers/dictionary.h"

const char todo_help_string[] =
"A todo-system for the command line.\n"
"   usage: %s <command> <args>\n"
"\n"
"List of commands used in c-todo:\n"
"\n"
"read commands\n"
"   show    Show todos (all or with given index/tag)\n"
"   find    Find todo with index or tag\n"
"   tags    List all tags\n"
"\n"
"write commands\n"
"   add     Add todo and tags\n"
"   remove  Remove todo with given index/tag\n"
"   edit    Edit todo with given index\n"
;

// #define DEBUG

#ifdef DEBUG
int main()
{
    char* argv[] = {
        "todo",
        "add",
        "A new task!",
        "#test",
        "#new"
    };
    int argc = 5;
#else
int main(int argc, char* argv[])
{
    if (argc < 2 || string_cmp(argv[1], "help"))
    {
        printf(todo_help_string, argv[0]);
        return 0;
    }
#endif
    // All the cleanup will be handled by the OS

    String filepath = "C:/todos/todo-data.txt";
    String contents = load_file(filepath);

    File_Data data = todos_file_make();
    todos_file_load(contents, &data);

    if (string_cmp(argv[1], "show"))
    {
        int show_tags = string_cmp(argv[2], "--with-tags");
        int subcmd_idx = show_tags ? 3 : 2;

        if (argc == subcmd_idx || string_cmp(argv[subcmd_idx], "all"))
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
                if (show_tags)
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
            if (argv[subcmd_idx][0] == '#')
            {
                Dict_Bkt(DArray(int)) bkt = dict_find(data.tag_dict, argv[subcmd_idx]);
                if (bkt == dict_end(data.tag_dict))
                {
                    printf("%s tag not found\n", argv[subcmd_idx]);
                    return 1;
                }

                da_foreach(int, it, bkt->value)
                {
                    printf("%d. %s", *it + 1, data.todos[*it].task);
                    if (show_tags)
                    {
                        printf(" :");
                        da_foreach(String, tag, data.todos[*it].tags)
                            printf(" %s", *tag);
                    }
                    printf("\n");
                }
            }
            else
            {
                int index = atoi(argv[subcmd_idx]);
                if (index <= 0 || index > da_size(data.todos))
                {
                    printf("There are only %zd todos\n", da_size(data.todos));
                    return 1;
                }

                printf("%d. %s", index, data.todos[index - 1].task);
                if (show_tags)
                {
                    printf(" :");
                    da_foreach(String, tag, data.todos[index - 1].tags)
                        printf(" %s", *tag);
                }
                printf("\n");
            }
        }

        return 0;
    }

    if (string_cmp(argv[1], "find"))
    {
        if (argc < 3)
        {
            printf("No term provided to find\n");
            return 1;
        }
        
        int size = da_size(data.todos);
        int count = 0;
        for (int i = 0; i < size; i++)
        {
            char* found = strstr(data.todos[i].task, argv[2]);
            if (found)
            {
                printf("%d. %s\n", i + 1, data.todos[i].task);
                count++;
            }
        }

        char s = (count == 1) ? '\0' : 's';
        printf("Found %d todo%c\n", count, s);
    
        return 0;
    }

    if (string_cmp(argv[1], "add"))
    {
        if (argc < 3)
        {
            printf("No task provided to add\n");
            return 1;
        }
        // @Todo: Check if task provided is empty

        Todo todo = todo_make(string_make(argv[2]));
        for (int i = 3; i < argc; i++)
        {
            // @Todo: Tags should start with '#'
            da_push_back(todo.tags, string_make(argv[i]));
        }

        todos_add_todo(&data, todo);
        String new_content = todos_generate_content(data);
        write_file(filepath, new_content);

        printf("Added todo: %s", todo.task);

        return 0;
    }

    if (string_cmp(argv[1], "remove"))
    {
        if (argc < 3)
        {
            printf("Provide index of todo to be removed\n");
            return 1;
        }

        if (string_cmp(argv[2], "all"))
        {
            // No need to free each todo
            // @Todo: No need to parse the file at all
            write_file(filepath, "");
            return 0;
        }

        if (argv[2][0] == '#')
        {
            // Remove by tag
            Dict_Bkt(DArray(int)) bkt = dict_find(data.tag_dict, argv[2]);
            if (bkt == dict_end(data.tag_dict))
            {
                printf("Tag not found: %s\n", argv[2]);
                return 1;
            }

            // Deleting in reverse order since the other way
            // round can cause errors due to change in order in todos array
            for (int i = da_size(bkt->value) - 1; i >= 0; i--)
            {
                int index = bkt->value[i];
                Todo todo = data.todos[index];
                da_erase_at(data.todos, (index));
                printf("Removed todo: %s\n", todo.task);
            }
        }
        else
        {
            // Remove by index
            int index = atoi(argv[2]);
            if (index <= 0 || index > da_size(data.todos))
            {
                printf("There are only %zd todos\n", da_size(data.todos));
                return 1;
            }

            Todo todo = data.todos[index - 1];
            da_erase_at(data.todos, (index - 1));
            printf("Removed todo: %s\n", todo.task);
        }

        String new_content = todos_generate_content(data);
        write_file(filepath, new_content);

        return 0;
    }

    if (string_cmp(argv[1], "edit"))
    {
        if (argc < 4)
        {
            printf("Expected index of todo to be edited\n");
            return 1;
        }

        int index = atoi(argv[2]);
        if (index <= 0 || index > da_size(data.todos))
        {
            printf("There are only %zd todos\n", da_size(data.todos));
            return 1;
        }

        Todo prev_todo = data.todos[index - 1];
        Todo new_todo = todo_make(string_make(argv[3]));

        for (int i = 4; i < argc; i++)
        {
            // @Todo: Tags should start with '#'
            da_push_back(new_todo.tags, string_make(argv[i]));
        }

        data.todos[index - 1] = new_todo;

        String new_content = todos_generate_content(data);
        write_file(filepath, new_content);

        printf("Todo updated\n");

        return 0;
    }

    if (string_cmp(argv[1], "tags"))
    {
        int tag_count = 0;
        dict_foreach(DArray(int), bkt, data.tag_dict)
        {
            if (bkt->key)
            {
                printf("%s (%zd)\n", bkt->key, da_size(bkt->value));
                tag_count++;
            }
        }

        char* new_line = (tag_count > 0) ? "\n" : "";
        printf("%sFound %d tag(s)\n", new_line, tag_count);

        return 0;
    }

    printf("Command not recognized\n");
    return 1;
}