#ifndef ARITHMETIC_H
    #define ARITHMETIC_H

    #include "stack.h"

    enum SpuBinaryMathOperations {
        BINARY_MATH_OPERATION_ADDITION,
        BINARY_MATH_OPERATION_SUBTRACTION,
        BINARY_MATH_OPERATION_MULTIPLICATION,
        BINARY_MATH_OPERATION_DIVISION,
    };

    enum SpuUnaryMathOperations {
        UNARY_MATH_OPERATION_SQUARE_ROOT,
        UNARY_MATH_OPERATION_SINUS,
        UNARY_MATH_OPERATION_COSINUS,
    };

    Error_t get_two_values(Stack * stk, Elem_t * val1, Elem_t * val2);
    Error_t spu_binary(Stack * stk, SpuBinaryMathOperations mode);
    Error_t spu_unary(Stack * stk, SpuUnaryMathOperations mode);

#endif
