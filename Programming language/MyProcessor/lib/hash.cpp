#include <stdio.h>
#include <math.h>

#include "hash.h"

#ifndef NHASHPOTECTION

    Hash_t calculate_hash(void * stk, const size_t size)
    {
        if (stk != nullptr)
        {
            char * pointer = (char *) stk;
            // char * start_pointer = pointer;
            char * end_pointer = (char *) stk + size - 1;
            Hash_t hash = 0;
            double value = 2;
            int start_pow = 8;
            Hash_t mod = 13371337;

            while (pointer < end_pointer)
            {
                hash += ((Hash_t) (pow(value, (double) (start_pow + (end_pointer - pointer))) * (*pointer))) % mod;

                pointer++;
            }

            return hash;
        }
        else
        {
            return 0;
        }
    }

#else // NHASHPROTECTION

    Hash_t calculate_hash(Stack * stk, const size_t size)
    {
        return (Hash_t) 0;
    }

#endif // NHASHPOTECTION
