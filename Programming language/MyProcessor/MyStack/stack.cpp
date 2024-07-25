#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "vtor.h"
#include "my_assert.h"
#include "hash.h"

enum StackResizes {
    STACK_EXPAND,
    STACK_CONSTRICT
};

static int stack_resize(Stack * stk, StackResizes resize_mode);
static Elem_t * stack_data_to_raw(Stack * stk);
static Elem_t * stack_raw_to_data(void * data);

const int STACK_EXPAND_COEFFICIENT = 2;
const int STACK_CONSTRICT_COEFFICIENT = 4;


Error_t stack_ctor(Stack * stk)
{
    MY_ASSERT(stk);

    Elem_t * tmp = NULL;
    Error_t errors = 0;

    if (stk->data != nullptr)
    {
        errors |= STACKERRORS_CANT_CONSTRUCT;

        return errors;
    }

    stk->size = 0;
    stk->capacity = STACK_START_CAPACITY;
    stk->left_jagajaga = STACK_JAGAJAGA_VALUE;
    stk->right_jagajaga = STACK_JAGAJAGA_VALUE;
    stk->hash = 0;


    if ((tmp = (Elem_t *) calloc(STACK_START_CAPACITY * sizeof(Elem_t) + 2 * sizeof(Jagajaga_t), sizeof(char))) == NULL)
    {
        stk->data = nullptr;

        errors |= STACKERRORS_CANT_ALLOCATE_MEMORY;

        return errors;
    }

    stk->data = stack_raw_to_data(tmp);

    *stack_get_data_left_jagajaga(stk) =  STACK_JAGAJAGA_VALUE;
    *stack_get_data_right_jagajaga(stk) = STACK_JAGAJAGA_VALUE;

    stk->hash = calculate_hash(stk, sizeof(Stack));
    stk->data_hash = calculate_hash(stk->data, stk->capacity * sizeof(Elem_t));
    errors = stack_vtor(stk);

    return errors;
}


Error_t stack_dtor(Stack * stk)
{
    MY_ASSERT(stk);

    Error_t errors = 0;

    if ((errors = stack_vtor(stk)))
    {
        return errors;
    }

    if (!stk->data && stk->capacity == STACK_POISON && stk->size == STACK_POISON)
    {
        errors |= STACKERRORS_CANT_DESTRUCT;

        return errors;
    }

    stk->capacity =       STACK_POISON;
    stk->size =           STACK_POISON;
    stk->hash =           STACK_POISON;
    stk->data_hash =      STACK_POISON;
    stk->left_jagajaga =  STACK_POISON;
    stk->right_jagajaga = STACK_POISON;

    free(stack_data_to_raw(stk));
    stk->data = nullptr;

    return errors;
}


Error_t stack_push(Stack * stk, const Elem_t value)
{
    MY_ASSERT(stk);

    stk->hash = stack_recalculate_hash(stk, sizeof(Stack));
    stk->data_hash = calculate_hash(stk->data, stk->capacity * sizeof(Elem_t));

    Error_t errors = stack_vtor(stk);

    if (errors)
    {
        show_dump(stk, &errors);

        return errors;
    }

    (stk->size)++;

    if (stk->size >= stk->capacity)
    {
        MY_ASSERT(stk->size == stk->capacity);

        if (stack_resize(stk, STACK_EXPAND))
        {
            errors |= STACKERRORS_CANT_ALLOCATE_MEMORY;

            return errors;
        }
    }

    stk->data[stk->size] = value;

    stk->hash = stack_recalculate_hash(stk, sizeof(Stack));
    stk->data_hash = calculate_hash(stk->data, stk->capacity * sizeof(Elem_t));

    return errors;
}


Error_t stack_pop(Stack * stk, Elem_t * value)
{
    MY_ASSERT(stk);
    MY_ASSERT(value);

    Error_t errors = stack_vtor(stk);

    if (errors)
    {
        return errors;
    }

    if (stk->size > 0)
    {
        *value = stk->data[stk->size];
        stk->data[stk->size] = STACK_POISON;
        (stk->size)--;

        if (stk->size <= stk->capacity / STACK_CONSTRICT_COEFFICIENT && stk->capacity > STACK_START_CAPACITY)
        {
            MY_ASSERT(stk->size == stk->capacity / STACK_CONSTRICT_COEFFICIENT);

            if (stack_resize(stk, STACK_CONSTRICT))
            {
                errors |= STACKERRORS_CANT_CONSTRICT;

                return errors;
            }
        }

        stk->hash = stack_recalculate_hash(stk, sizeof(Stack));
        stk->data_hash = calculate_hash(stk->data, stk->capacity * sizeof(Elem_t));
    }
    else
    {
        errors |= STACKERRORS_EMPTY_STACK;
    }

    return errors;
}

       // Error_t
static int stack_resize(Stack * stk, StackResizes resize_mode)
{
    MY_ASSERT(stk);

    Elem_t * pointer = NULL;

    if (resize_mode == STACK_EXPAND)
    {
        *stack_get_data_right_jagajaga(stk) = 0;

        if (!(pointer = (Elem_t *) realloc(stack_data_to_raw(stk), (stk->capacity * sizeof(Elem_t)) * STACK_EXPAND_COEFFICIENT + (2 * sizeof(Jagajaga_t)))))
        {
            free(stack_data_to_raw(stk));
            return 1;
        }
    }
    else // resize_mode == STACK_CONSTRICT
    {
        if (!(pointer = (Elem_t *) realloc(stack_data_to_raw(stk), (stk->capacity * sizeof(Elem_t)) / STACK_EXPAND_COEFFICIENT + (2 * sizeof(Jagajaga_t)))))
        {
            free(stack_data_to_raw(stk));
            return 1;
        }
    }


    stk->data = stack_raw_to_data(pointer);
    resize_mode ? (stk->capacity /= STACK_EXPAND_COEFFICIENT) : (stk->capacity *= STACK_EXPAND_COEFFICIENT);

    *stack_get_data_right_jagajaga(stk) = STACK_JAGAJAGA_VALUE;

    return 0;
}


Jagajaga_t * stack_get_data_left_jagajaga(const Stack * stk)
{
    return (stk->data ? ((Jagajaga_t *) stk->data - 1) : 0);
}


Jagajaga_t * stack_get_data_right_jagajaga(const Stack * stk)
{
    return (stk->data ? ((Jagajaga_t *) (stk->data + stk->capacity)) : 0);
}


static Elem_t * stack_data_to_raw(Stack * stk)
{
    #ifndef NCANARYPROTECTION
        return (Elem_t *) ((Jagajaga_t *) stk->data - 1);
    #elif
        return (Elem_t *) data;
    #endif // NCANARYPROTECTION
}


static Elem_t * stack_raw_to_data(void * data)
{
    #ifndef NCANARYPROTECTION
        return (Elem_t *) ((Jagajaga_t *) data + 1);
    #elif
        return (Elem_t) data;
    #endif // NCANARYPROTECTION
}
