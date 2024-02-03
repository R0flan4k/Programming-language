#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "front_end.h"
#include "my_assert.h"

const size_t MAX_STRING_SIZE = 128;

static char * fe_next_token(char * buffer);
static bool is_operator(char * buffer, size_t * index);
static bool is_key_word(char * buffer, size_t * index);


FrontEndErrors separate_tokens(char * buffer, FrontEndToken * tokens, size_t max_tokens_number,
                               NameTable * name_table)
{
    MY_ASSERT(buffer);

    FrontEndErrors fe_errors = 0;
    char * buffer_ptr = buffer;
    double number = 0;
    int length = 0;
    size_t arrays_index = 0;
    char string[MAX_STRING_SIZE] = "";

    buffer_ptr = fe_next_token(buffer_ptr);

    size_t index = 0;
    while (*buffer_ptr != '\0')
    {
        if (index >= max_tokens_number - 1)
        {
            fe_errors |= FRONT_END_ERRORS_TOKENS_ARRAY_OVERFLOW;
            return fe_errors;
        }

        if (isdigit(*buffer_ptr))
        {
            sscanf(buffer_ptr, "%lf%n", &number, &length);

            tokens[index].type = TOKEN_TYPES_NUMBER;
            tokens[index].val.num = number;

            buffer_ptr += length;
        }
        else if (is_braket(*buffer_ptr))
        {
            tokens[index].type = TOKEN_TYPES_SYMBOL;
            tokens[index].val.sym = *buffer_ptr;

            buffer_ptr++;
        }
        else if (is_operator(buffer_ptr, &arrays_index))
        {
            tokens[index].type = TOKEN_TYPES_OPERATOR;
            tokens[index].val.op = OPERATORS_ARRAY[arrays_index];

            buffer_ptr += OPERATORS_ARRAY[arrays_index].length;
        }
        else if (is_key_word(buffer_ptr, &arrays_index))
        {
            tokens[index].type = TOKEN_TYPES_KEY_WORD;
            tokens[index].val.kwd = KEY_WORDS_ARRAY[arrays_index];

            buffer_ptr += KEY_WORDS_ARRAY[arrays_index].length;
        }
        else
        {
            sscanf(buffer_ptr, "%s%n", string, &length);

            if (!isspace(*(buffer_ptr + length)))
            {
                printf("%d %c\n", *(buffer_ptr + length), *(buffer_ptr + length));
                printf("%c %d\n", *buffer_ptr, length);
                fe_errors |= FRONT_END_ERRORS_CANT_SEPARATE_NAME_TABLE_ELEM;
                return fe_errors;
            }

            *(buffer_ptr + length) = '\0';

            if (!is_in_name_table(name_table, buffer_ptr, &arrays_index))
            {
                if (!add_name_table_elem(name_table, buffer_ptr, length, &arrays_index))
                {
                    fe_errors |= FRONT_END_ERRORS_NAME_TABLE_OVERFLOW;
                    return fe_errors;
                }
            }

            tokens[index].type = TOKEN_TYPES_NAME_TABLE_ELEM;
            tokens[index].val.elem = name_table->names[arrays_index];

            buffer_ptr += length + 1;
        }

        index++;

        buffer_ptr = fe_next_token(buffer_ptr);
    }

    tokens[index].type = TOKEN_TYPES_TERMINATOR;
    tokens[index].val.sym = '\0';

    return fe_errors;
}


FrontEndErrors get_program_code(FrontEndToken * tokens, NameTable * name_table, Tree * program_tree)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);

    size_t token_index = 0;

    FrontEndErrors fe_errors = get_all_functions(tokens, name_table, &token_index);

    if (tokens[token_index].type != TOKEN_TYPES_TERMINATOR ||
        tokens[token_index].val.sym != '\0')
    {
        fe_errors |= FRONT_END_ERRORS_CANT_FIND_TERMINATOR;
    }

    return fe_errors;
}


static FrontEndErrors get_all_functions(FrontEndToken * tokens, NameTable * name_table,
                                        size_t * token_index, Tree * program_tree)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);

    if (!is_function(tokens, name_table, token_index))
    {
        return FRONT_END_ERRORS_NO_FUNCTIONS;
    }

    FrontEndErrors fe_errors = get_function(tokens, name_table, token_index,
                                            program_tree, &(program_tree->root));
    TreeNode * * cur_node = &(program_tree->root->right);

    if (fe_errors)
    {
        return fe_errors;
    }

    while (is_function(tokens, name_table, token_index))
    {
        fe_errors |= get_function(tokens, name_table, token_index,
                                  program_tree, cur_node);
        cur_node = &((*cur_node)->right);

        if (fe_errors)
        {
            return fe_errors;
        }
    }

    return fe_errors;
}


static bool is_function(FrontEndToken * tokens, NameTable * name_table, size_t * token_index)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);

    if (tokens[*token_index].type == TOKEN_TYPES_KEY_WORD &&
        tokens[*token_index].val.kwd.id == KEY_WORDS_FUNCTION_START)
    {
        (*token_index)++;
        return true;
    }

    return false;
}


static FrontEndErrors get_function(FrontEndToken * tokens, NameTable * name_table, size_t * token_index,
                                   Tree * program_tree, TreeNode * * func_root)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);

    FrontEndErrors fe_errors = 0;

    (*token_index)++;

    const char * return_type = NULL;
    fe_errors |= get_type(tokens, token_index, return_type);

    if (fe_errors)
    {
        return fe_errors;
    }

    if (tokens[*token_index].type != TOKEN_TYPES_NAME_TABLE_ELEM)
    {
        fe_errors |= FRONT_END_ERRORS_INVALID_FUNCTION_NAME;
        return fe_errors;
    }

    Tree tmp_tree = {};

    Tree_t root_value = {.val = {.string = tokens[*token_index].val.elem.name},
                         .type = TREE_NODE_TYPES_STRING};
    TError_t tree_errors = op_new_tree(&tmp_tree, root_value);

    Tree_t left_value = {.val = {.string = return_type},
                         .type = TREE_NODE_TYPES_STRING};
    tree_errors |= tree_insert(&tmp_tree, tmp_tree.root, TREE_NODE_BRANCH_LEFT, left_value);

    (*token_index)++;

    #if 0
    if (tokens[*token_index].type != TOKEN_TYPES_SYMBOL ||
        tokens[*token_index].val.sym != '(')
    {
        error check
    }
    #endif

    (*token_index)++;
    const char * argument_type = NULL;
    TreeNode * cur_node = tmp_tree.root->left;

    while (tokens[*token_index].type == TOKEN_TYPES_KEY_WORD)
    {
        MY_ASSERT(!cur_node);

        fe_errors |= get_type(tokens, token_index, argument_type);
        left_value = {.val = {.string = argument_type},
                      .type = TREE_NODE_TYPES_STRING};
        tree_errors |= tree_insert(&tmp_tree, cur_node, TREE_NODE_BRANCH_LEFT, left_value);

        left_value = {.val = {.string = tokens[*token_index].val.elem.name},
                      .type = TREE_NODE_TYPES_STRING};
        (*token_index)++;
        tree_errors |= tree_insert(&tmp_tree, cur_node->left, TREE_NODE_BRANCH_RIGHT, left_value);

        cur_node = cur_node->left;
    }

    #if 0
    if (tokens[*token_index].type != TOKEN_TYPES_SYMBOL ||
        tokens[*token_index].val.sym != ')')
    {
        error check
    }
    #endif

    (*token_index)++;

    #if 0
    if (tokens[*token_index].type != TOKEN_TYPES_SYMBOL ||
        tokens[*token_index].val.sym != '{')
    {
        error check
    }
    #endif

    (*token_index)++;

    fe_errors |= get_operator(tokens, name_table, token_index,
                              program_tree, tmp_tree.root->left->right);

    if (fe_errors)
        return fe_errors;

    #if 0
    if (tokens[*token_index].type != TOKEN_TYPES_SYMBOL ||
        tokens[*token_index].val.sym != '}')
    {
        error check
    }
    #endif

    (*token_index)++;

    return fe_errors;
}


static FrontEndErrors get_type(FrontEndToken * tokens, size_t * token_index,
                               const char * type_str)
{
    MY_ASSERT(tokens);
    MY_ASSERT(token_index);
    MY_ASSERT(type_str);

    FrontEndErrors fe_errors = 0;

    if (tokens[*token_index].type != TOKEN_TYPES_KEY_WORD)
    {
        fe_errors |= FRONT_END_ERRORS_INVALID_RETURN_TYPE;
        return fe_errors;
    }

    type_str = tokens[*token_index].val.kwd.name;
    (*token_index)++;

    while (tokens[*token_index].type == TOKEN_TYPES_KEY_WORD)
    {
        delete_terminator(const_cast <char *> (type_str));
        (*token_index)++;
    }

    return fe_errors;
}


static char * fe_next_token(char * buffer)
{
    MY_ASSERT(buffer);

    while (isspace(*buffer) && *buffer != '\0' &&
           !is_braket(*buffer) && !is_operator(buffer, NULL))
        buffer++;

    return buffer;
}


static bool is_operator(char * buffer, size_t * index)
{
    MY_ASSERT(buffer);

    bool success = false;

    size_t i = 0;
    for (i = 0; i < OPERATORS_ARRAY_SIZE; i++)
    {
        if (strncmp(OPERATORS_ARRAY[i].value, buffer, OPERATORS_ARRAY[i].length) == 0)
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


static bool is_key_word(char * buffer, size_t * index)
{
    MY_ASSERT(buffer);

    bool success = false;

    size_t i = 0;
    for (i = 0; i < KEY_WORDS_ARRAY_SIZE; i++)
    {
        if (strncmp(KEY_WORDS_ARRAY[i].name, buffer, KEY_WORDS_ARRAY[i].length) == 0)
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


void tokens_dump(const FrontEndToken * tokens)
{
    MY_ASSERT(tokens);

    printf("----------------------------------------------\n");

    size_t i = 0;
    for (i = 0; tokens[i].type != TOKEN_TYPES_TERMINATOR &&
         i < MAX_TOKENS_NUMBER; i++)
    {
        switch (tokens[i].type)
        {
            case TOKEN_TYPES_NUMBER:
                printf("[%zd]\t %-25lf | NUMBER\n", i, tokens[i].val.num);
                break;

            case TOKEN_TYPES_OPERATOR:
                printf("[%zd]\t %-25s | OPERATOR\n", i, tokens[i].val.op.value);
                break;

            case TOKEN_TYPES_KEY_WORD:
                printf("[%zd]\t %-25s | KEY WORD\n", i, tokens[i].val.kwd.name);
                break;

            case TOKEN_TYPES_SYMBOL:
                printf("[%zd]\t %-25c | SYMBOL\n", i, tokens[i].val.sym);
                break;

            case TOKEN_TYPES_NAME_TABLE_ELEM:
                printf("[%zd]\t %-25s | NAME TABLE ELEMENT\n", i, tokens[i].val.elem.name);
                break;

            case TOKEN_TYPES_TERMINATOR:
                break;

            default:
                MY_ASSERT(0 && "UNREACHABLE");
                break;
        }
    }

    printf("[%zd]\t \\0                        | TERMINATOR\n", i);

    printf("----------------------------------------------\n");
}
