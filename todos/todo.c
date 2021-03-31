#include "todo.h"

Todo todo_make(String task)
{
    Todo todo = { 0 };
    todo.task = task;
    da_make(todo.tags);
    return todo;
}

void todo_free(Todo* todo)
{
    if (todo->task)
        string_free(&todo->task);

    da_foreach(String, tag, todo->tags)
        string_free(tag);
    da_free(todo->tags);
}

File_Data todos_file_make()
{
    File_Data data = { 0 };

    da_make(data.todos);
    dict_make(data.tag_dict);

    return data;
}

static void consume_ws(String string, int* index)
{
    while (string[*index] == ' '  ||
           string[*index] == '\t' ||
           string[*index] == '\r' ||
           string[*index] == '\n')
    *index = *index + 1;
}

void todos_file_load(String contents, File_Data* data)
{
    int curr_index = 0;
    for (int i = 0; contents[i]; i++)
    {
        consume_ws(contents, &i);
        if (!contents[i])
            break;

        String task = string_make_till_char(contents + i, '#');
        i += string_length(task);

        Todo todo = todo_make(task);

        int j = i + 1;
        while (contents[j] != '\r' &&
               contents[j] != '\n' &&
               contents[j] != '\0')
        {
            if (contents[j] == '#')
            {
                String tag = string_make_till_n(contents + i, j - i);
                da_push_back(todo.tags, tag);
                i = j;

                Dict_Bkt(DArray(int)) bkt = dict_find(data->tag_dict, tag);

                if (bkt == dict_end(data->tag_dict))
                {
                    DArray(int) list = NULL;
                    da_make(list);
                    da_push_back(list, curr_index);
                    dict_put(data->tag_dict, tag, list);
                }
                else
                    da_push_back(bkt->value, curr_index);
            }

            j++;
        }

        da_push_back(data->todos, todo);
        curr_index++;
    }
}

static void push_string(DArray(char)* buffer, String str)
{
    for (int i = 0; str[i]; i++)
        da_push_back((*buffer), str[i]);
}

String todos_generate_content(File_Data data)
{
    if (da_size(data.todos) == 0)
        return NULL;

    DArray(char) buffer = NULL;
    da_make(buffer);

    da_foreach(Todo, todo, data.todos)
    {
        push_string(&buffer, todo->task);
        da_foreach(String, tag, todo->tags)
            push_string(&buffer, *tag);

        da_push_back(buffer, '#');
        da_push_back(buffer, '\r');
        da_push_back(buffer, '\n');
    }

    da_push_back(buffer, '\0');

    String res = string_make(buffer);
    da_free(buffer);

    return res;
}

void todos_file_free(File_Data* data)
{
    da_foreach(Todo, todo, data->todos)
        todo_free(todo);
    da_free(data->todos);

    dict_foreach(Todo*, it, data->tag_dict)
    {
        if (it->key)
            string_free(&it->key);
        if (it->value)
            da_free(it->value);
    }
    dict_free(data->tag_dict);
}

void todos_add_todo(File_Data* data, Todo todo)
{
    int todo_index = da_size(data->todos);
    da_push_back(data->todos, todo);
}