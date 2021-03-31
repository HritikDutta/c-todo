#pragma once

#include "containers/string.h"
#include "containers/darray.h"
#include "containers/dictionary.h"

typedef struct
{
    String task;
    DArray(String) tags;
} Todo;

// Moves task string into the todo object.
// Don't free the task string after calling this.
Todo todo_make(String task);
void todo_free(Todo* todo);

typedef struct
{
    DArray(Todo) todos;
    Dict(DArray(int)) tag_dict;
} File_Data;

File_Data todos_file_make();
void todos_file_load(String contents, File_Data* data);
String todos_generate_content(File_Data data);
void todos_file_free(File_Data* data);

void todos_add_todo(File_Data* data, Todo todo);