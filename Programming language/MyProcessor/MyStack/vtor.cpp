#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "vtor.h"
#include "my_assert.h"
#include "cmd_input.h"
#include "hash.h"

struct StackError {
    StackErrorsMasks mask;
    const char * output_error;
};

StackError INVALID_SIZE = {
    .mask = STACKERRORS_INVALID_SIZE,
    .output_error = "Invlid size.\n"
};

StackError INVALID_CAPACITY = {
    .mask = STACKERRORS_INVALID_CAPACITY,
    .output_error = "Invlid capacity.\n"
};

StackError INVALID_SIZECAPACITY = {
    .mask = STACKERRORS_INVALID_SIZECAPACITY,
    .output_error = "Capacity less than size.\n"
};

StackError INVALID_DATA = {
    .mask = STACKERRORS_INVALID_DATA,
    .output_error = "Data is nullptr.\n"
};

StackError CANT_ALLOCATE_MEMORY = {
    .mask = STACKERRORS_CANT_ALLOCATE_MEMORY,
    .output_error = "Can't allocate a memory.\n"
};

StackError CANT_CONSTRUCT = {
    .mask = STACKERRORS_CANT_CONSTRUCT,
    .output_error = "Can't construct a constructed stack.\n"
};

StackError CANT_DESTRUCT = {
    .mask = STACKERRORS_CANT_DESTRUCT,
    .output_error = "Can't destruct a destructed stack.\n"
};

StackError CANT_CONSTRICT = {
    .mask = STACKERRORS_CANT_CONSTRICT,
    .output_error = "Can't constrict the stack capacity.\n"
};

StackError EMPTY_STACK = {
    .mask = STACKERRORS_EMPTY_STACK,
    .output_error = "Can't pop an empty stack.\n"
};

StackError SPOILED_LEFT_JAGAJAGA = {
    .mask = STACKERRORS_SPOILED_LEFT_JAGAJAGA,
    .output_error = "Spoiled left canary.\n"
};

StackError SPOILED_RIGHT_JAGAJAGA = {
    .mask = STACKERRORS_SPOILED_RIGHT_JAGAJAGA,
    .output_error = "Spoiled right canary.\n"
};

StackError SPOILED_DATA_LEFT_JAGAJAGA = {
    .mask = STACKERRORS_SPOILED_DATA_LEFT_JAGAJAGA,
    .output_error = "Spoiled left data canary.\n"
};

StackError SPOILED_DATA_RIGHT_JAGAJAGA = {
    .mask = STACKERRORS_SPOILED_DATA_RIGHT_JAGAJAGA,
    .output_error = "Spoiled right data canary.\n"
};

StackError SPOILED_HASH_VALUE = {
    .mask = STACKERRORS_SPOILED_HASH_VALUE,
    .output_error = "Spoiled hash value.\n"
};

StackError SPOILED_DATA_HASH_VALUE = {
    .mask = STACKERRORS_SPOILED_DATA_HASH_VALUE,
    .output_error = "Spoiled data hash value.\n"
};


Error_t stack_vtor(Stack * const stk)
{
    Error_t verificator = 0;
    Hash_t stack_tmp_hash = stk->hash;
    Hash_t data_tmp_hash =  stk->data_hash;

    if (stk->size < 0)              verificator |= STACKERRORS_INVALID_SIZE;
    if (stk->capacity < 0)          verificator |= STACKERRORS_INVALID_CAPACITY;
    if (stk->size >= stk->capacity) verificator |= STACKERRORS_INVALID_SIZECAPACITY;
    if (stk->left_jagajaga != STACK_JAGAJAGA_VALUE)            verificator |= STACKERRORS_SPOILED_LEFT_JAGAJAGA;
    if (stk->right_jagajaga != STACK_JAGAJAGA_VALUE)           verificator |= STACKERRORS_SPOILED_RIGHT_JAGAJAGA;
    if (stack_tmp_hash != stack_recalculate_hash(stk, sizeof(Stack))) verificator |= STACKERRORS_SPOILED_HASH_VALUE;

    if (!stk->data)
    {
        verificator |= STACKERRORS_INVALID_DATA;
        return verificator;
    }
    if (*stack_get_data_left_jagajaga(stk) != STACK_JAGAJAGA_VALUE)  verificator |= STACKERRORS_SPOILED_DATA_LEFT_JAGAJAGA;
    if (*stack_get_data_right_jagajaga(stk) != STACK_JAGAJAGA_VALUE) verificator |= STACKERRORS_SPOILED_DATA_RIGHT_JAGAJAGA;
    if (data_tmp_hash != calculate_hash(stk->data, stk->capacity *  sizeof(Elem_t))) verificator |= STACKERRORS_SPOILED_DATA_HASH_VALUE;

    stk->data_hash = data_tmp_hash;
    stk->hash = stack_tmp_hash;

    return verificator;
}


void show_dump_basis(const Stack * stk, const char * stack_name, const Error_t * verificator, const char * func, const int line, const char * file)
{
    printf("-------------------------------------------------------------------\n");
    printf("Stack[%p] \"%s\" from %s(%d), %s\n", stk, stack_name, file, line, func);
    printf("{\n");

    printf("    hash =              %lld%s\n",
                stk->hash,     stk->hash == STACK_POISON    ? "(POISON)" : "");
    printf("    data hash =         %lld%s\n",
                stk->data_hash, stk->data_hash == STACK_POISON ? "(POISON)" : "");
    printf("    left canary =       %s%s\n",
                stk->left_jagajaga == STACK_JAGAJAGA_VALUE  ? "verified" : "spoiled",
                stk->left_jagajaga == STACK_POISON ? "(POISON)" : "");
    printf("    right canary =      %s%s\n",
                stk->right_jagajaga == STACK_JAGAJAGA_VALUE ? "verified" : "spoiled",
                stk->left_jagajaga == STACK_POISON ? "(POISON)" : "");

    if (stk->data != nullptr)
    {
    printf("    left data canary =  %s%s\n",
                *stack_get_data_left_jagajaga(stk) == STACK_JAGAJAGA_VALUE ? "verified" : "spoiled",
                *stack_get_data_left_jagajaga(stk) == STACK_POISON ? "(POISON)" : "");
    printf("    right data canary = %s%s\n",
                *stack_get_data_right_jagajaga(stk) == STACK_JAGAJAGA_VALUE ? "verified" : "spoiled",
                *stack_get_data_right_jagajaga(stk) == STACK_POISON ? "(POISON)" : "");
    }

    printf("    size =     %d%s\n",   stk->size,     stk->size == STACK_POISON     ? "(POISON)" : "");
    printf("    capacity = %d%s\n",   stk->capacity, stk->capacity == STACK_POISON ? "(POISON)" : "");
    printf("    data[%p%s]\n",        stk->data,     stk->data == STACK_POISON_PTR ? "(POISON)" : "");
    printf("    {\n");

    int i = 1;

    while (i <= stk->size && stk->data != nullptr)
    {
        printf("        [%d] = " ELEM_SPEC "\n", i, stk->data[i]);

        i++;
    }

    printf("    }\n");
    printf("}\n");

    if (*verificator)
    {
        StackError const * errors[] = {
            &(INVALID_SIZE),               &(INVALID_CAPACITY),
            &(INVALID_SIZECAPACITY),       &(INVALID_DATA),
            &(CANT_ALLOCATE_MEMORY),       &(CANT_DESTRUCT),
            &(CANT_CONSTRICT),             &(EMPTY_STACK),
            &(SPOILED_LEFT_JAGAJAGA),      &(SPOILED_RIGHT_JAGAJAGA),
            &(SPOILED_DATA_LEFT_JAGAJAGA), &(SPOILED_DATA_RIGHT_JAGAJAGA),
            &(SPOILED_HASH_VALUE),         &(SPOILED_DATA_HASH_VALUE),
            &(CANT_CONSTRUCT)
        };

        printf("Errors:\n");

        size_t array_size = sizeof(errors) / sizeof(errors[0]);
        size_t j = 0;

        while (j < array_size)
        {
            if (errors[j]->mask & *verificator)
                fprintf(stderr, RED_COLOR "%s" DEFAULT_COLOR, errors[j]->output_error);

            j++;
        }
    }

    printf("-------------------------------------------------------------------\n");
}
