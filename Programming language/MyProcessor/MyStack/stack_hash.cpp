#include <stdio.h>
#include <math.h>

#include "stack.h"
#include "hash.h"

#ifndef NHASHPOTECTION

    Hash_t stack_recalculate_hash(Stack * stk, const size_t size)
    {
        if (stk != nullptr)
        {
            stk->hash = 0;
            stk->data_hash = 0;

            return calculate_hash(stk, size);
        }
        else
        {
            return 0;
        }
    }

#else // NHASHPROTECTION

    Hash_t stack_recalculate_hash(Stack * stk, const size_t size)
    {
        return (Hash_t) 0;
    }

#endif // NHASHPOTECTION
