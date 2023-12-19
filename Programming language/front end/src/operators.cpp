#include "operators.h"
#include "my_assert.h"

static size_t len(size_t size);
static Operator create_operator(const char * op_value, size_t op_length, OperatorIDs op_id);

Operator OPERATORS_ARRAY[] = {
    create_operator("+",  len(sizeof("+")),  OPERATORS_ADDITION),
    create_operator("-",  len(sizeof("-")),  OPERATORS_SUBTRACTION),
    create_operator("*",  len(sizeof("*")),  OPERATORS_MULTIPLICATION),
    create_operator("/",  len(sizeof("/")),  OPERATORS_DIVISION),
    create_operator("^",  len(sizeof("^")),  OPERATORS_POWER),
    create_operator("=",  len(sizeof("=")),  OPERATORS_ASSIGMENT),
    create_operator(";",  len(sizeof(";")),  OPERATORS_END_OF_OP),
};
size_t OPERATORS_ARRAY_SIZE = sizeof(OPERATORS_ARRAY) / sizeof(OPERATORS_ARRAY[0]);

static Operator create_operator(const char * op_value, size_t op_length, OperatorIDs op_id)
{
    MY_ASSERT(op_value);

    Operator cur_operator = {
        .value = op_value,
        .length = op_length,
        .id = op_id,
    };

    return cur_operator;
}


static size_t len(size_t size)
{
    return size - 1;
}
