#ifndef OPERATORS_H
    #define OPERATORS_H

    #include <stdio.h>

    enum OperatorIDs {
        OPERATORS_INVALID,
        OPERATORS_ADDITION,
        OPERATORS_SUBTRACTION,
        OPERATORS_MULTIPLICATION,
        OPERATORS_DIVISION,
        OPERATORS_POWER,
        OPERATORS_ASSIGMENT,
        OPERATORS_END_OF_OP,
        OPERATORS_BELOW,
        OPERATORS_ABOVE,
        OPERATORS_EQUAL,
    };

    struct Operator {
        const char * value;
        size_t length;
        OperatorIDs id;
    };

    extern Operator OPERATORS_ARRAY[];
    extern size_t OPERATORS_ARRAY_SIZE;

#endif // OPERATORS_H
