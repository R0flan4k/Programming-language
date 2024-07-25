#include <string.h>

#include "name_table.h"
#include "my_assert.h"


bool is_in_name_table(const NameTable * name_table, const char * name, size_t * index)
{
    MY_ASSERT(name);
    MY_ASSERT(name_table);

    bool success = false;

    size_t i = 0;
    for (i = 0; i < name_table->size; i++)
    {
        if (!strcmp(name, name_table->names[i].name))
        {
            success = true;
            break;
        }
    }

    if (index && success)
    {
        *index = i;
    }

    return success;
}


bool add_name_table_elem(NameTable * name_table, const char * name, const size_t length, size_t * index)
{
    MY_ASSERT(name_table);
    MY_ASSERT(name);

    if (name_table->size >= NAME_TABLE_CAPACITY - 1)
    {
        return false;
    }

    size_t i = name_table->size;

    name_table->size++;
    name_table->names[i].name = name;
    name_table->names[i].length = length;

    if (index)
    {
        *index = i;
    }

    return true;
}
