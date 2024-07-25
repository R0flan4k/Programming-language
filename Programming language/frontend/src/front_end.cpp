#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "front_end.h"
#include "my_assert.h"

const size_t MAX_STRING_SIZE = 128;
const char * TREE_FILE_NAME = "feprog";
const char * START_FUNCTION_NAME = "SHypaashkaaraa";
const size_t START_FUNCTION_LEN = sizeof("SHypaashkaaraa") - 1;

static FrontEndErrors get_all_functions(LangToken * tokens, NameTable * name_table,
                                        size_t * token_index, Tree * program_tree);
static FrontEndErrors get_function(LangToken * tokens, NameTable * name_table,
                                   size_t * token_index, Tree * program_tree,
                                   TreeNode * function_node);
static FrontEndErrors get_func_args(LangToken * tokens, NameTable * name_table,
                                    size_t * token_index, Tree * program_tree,
                                    TreeNode * arg_node, bool * defined_vars);
static FrontEndErrors get_def_arg(LangToken * tokens, NameTable * name_table,
                                  size_t * token_index, Tree * program_tree,
                                  TreeNode * arg_node, bool * defined_vars);
static FrontEndErrors get_func_body(LangToken * tokens, NameTable * name_table,
                                    size_t * token_index, Tree * program_tree,
                                    TreeNode * body_node, bool * defined_vars);
static FrontEndErrors get_ret(LangToken * tokens, NameTable * name_table,
                              size_t * token_index, Tree * program_tree,
                              TreeNode * node, bool * defined_vars);
static FrontEndErrors get_calc_op(LangToken * tokens, NameTable * name_table,
                                  size_t * token_index, Tree * program_tree,
                                  TreeNode * node, bool * defined_vars);
static FrontEndErrors get_definition(LangToken * tokens, NameTable * name_table,
                                     size_t * token_index, Tree * program_tree,
                                     TreeNode * node, bool * defined_vars);
static FrontEndErrors get_if(LangToken * tokens, NameTable * name_table,
                             size_t * token_index, Tree * program_tree,
                             TreeNode * cur_node, bool * defined_vars);
static FrontEndErrors get_while(LangToken * tokens, NameTable * name_table,
                                size_t * token_index, Tree * program_tree,
                                TreeNode * cur_node, bool * defined_vars);
static FrontEndErrors get_expression(LangToken * tokens, NameTable * name_table,
                                     size_t * token_index, Tree * program_tree,
                                     TreeNode * exp_node, bool * defined_vars);
static FrontEndErrors get_e(LangToken * tokens, NameTable * name_table,
                            size_t * token_index, Tree * program_tree,
                            TreeNode * node, bool * defined_vars);
static FrontEndErrors get_tp(LangToken * tokens, NameTable * name_table,
                             size_t * token_index, Tree * program_tree,
                             TreeNode * node, bool * defined_vars);
static FrontEndErrors get_p(LangToken * tokens, NameTable * name_table,
                            size_t * token_index, Tree * program_tree,
                            TreeNode * node, bool * defined_vars);
static FrontEndErrors get_n(LangToken * tokens, NameTable * name_table,
                            size_t * token_index, Tree * program_tree,
                            TreeNode * node, bool * defined_vars);
static FrontEndErrors get_func_call(LangToken * tokens, NameTable * name_table,
                                    size_t * token_index, Tree * program_tree,
                                    TreeNode * node, bool * defined_vars);
static FrontEndErrors get_func_call_arg(LangToken * tokens, NameTable * name_table,
                                        size_t * token_index, Tree * program_tree,
                                        TreeNode * node, bool * defined_vars);
static FrontEndErrors get_out(LangToken * tokens, NameTable * name_table,
                              size_t * token_index, Tree * program_tree,
                              TreeNode * node, bool * defined_vars);
static FrontEndErrors get_sqrt(LangToken * tokens, NameTable * name_table,
                               size_t * token_index, Tree * program_tree,
                               TreeNode * node, bool * defined_vars);

static char * fe_next_token(char * buffer);
static bool is_function(LangToken * token);
static bool is_data_type(LangToken * token);
static bool is_func_call(LangToken * tokens, size_t token_index);
static bool is_func_call_arg(LangToken * token);
static bool is_e_op(LangToken * token);
static bool is_tp_op(LangToken * token);
static bool is_ret(LangToken * token);
static bool is_calc_op(LangToken * tokens, size_t token_index);
static bool is_if(LangToken * token);
static bool is_in(LangToken * token);
static bool is_out(LangToken * token);
static bool is_sqrt(LangToken * token);
static bool is_while(LangToken * token);
static bool is_operator(char * buffer, size_t * index);
static bool is_key_word(char * buffer, size_t * index);
static bool is_spec_symbol(char symbol);
static bool is_comprassion_op(LangToken * token);

#ifndef NDEBUG
    static void tok_dump(LangToken * token);
#endif // NDEBUG


FrontEndErrors separate_tokens(char * buffer, LangToken * tokens,
                               size_t max_tokens_number, NameTable * name_table)
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
        else if (is_spec_symbol(*buffer_ptr))
        {
            tokens[index].type = TOKEN_TYPES_SYMBOL;
            tokens[index].val.sym = *buffer_ptr;

            buffer_ptr++;
        }
        else if (is_operator(buffer_ptr, &arrays_index))
        {
            tokens[index].type = TOKEN_TYPES_OPERATOR;
            tokens[index].val.op_id = arrays_index;

            buffer_ptr += OPERATORS_ARRAY[arrays_index].length;
        }
        else if (is_key_word(buffer_ptr, &arrays_index))
        {
            tokens[index].type = TOKEN_TYPES_KEY_WORD;
            tokens[index].val.kwd_id = arrays_index;

            buffer_ptr += KEY_WORDS_ARRAY[arrays_index].length;
        }
        else
        {
            sscanf(buffer_ptr, "%s%n", string, &length);

            if (!isspace(*(buffer_ptr + length)))
            {
                fe_errors |= FRONT_END_ERRORS_CANT_SEPARATE_NAME_TABLE_ELEM;
                return fe_errors;
            }

            *(buffer_ptr + length) = '\0';

            if (!is_in_name_table(name_table, buffer_ptr, &arrays_index))
            {
                if (!add_name_table_elem(name_table, buffer_ptr,
                                         length, &arrays_index))
                {
                    fe_errors |= FRONT_END_ERRORS_NAME_TABLE_OVERFLOW;
                    return fe_errors;
                }
            }

            tokens[index].type = TOKEN_TYPES_NAME_TABLE_ELEM;
            tokens[index].val.elem_id = arrays_index;

            buffer_ptr += length + 1;
        }

        index++;

        buffer_ptr = fe_next_token(buffer_ptr);
    }

    tokens[index].type = TOKEN_TYPES_TERMINATOR;
    tokens[index].val.sym = '\0';

    return fe_errors;
}


FrontEndErrors get_program_code(LangToken * tokens, NameTable * name_table,
                                Tree * program_tree, size_t * token_index)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);

    FrontEndErrors fe_errors = get_all_functions(tokens, name_table,
                                                 token_index, program_tree);
    if (fe_errors) return fe_errors;

    if (tokens[*token_index].type != TOKEN_TYPES_TERMINATOR ||
        tokens[*token_index].val.sym != '\0')
    {
        fe_errors |= FRONT_END_ERRORS_CANT_FIND_TERMINATOR;
    }

    return fe_errors;
}


static FrontEndErrors get_all_functions(LangToken * tokens, NameTable * name_table,
                                        size_t * token_index, Tree * program_tree)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);
    MY_ASSERT(program_tree);

    if (!is_function(tokens + *token_index))
    {
        return FRONT_END_ERRORS_NO_FUNCTIONS;
    }
    (*token_index)++;

    TreeNode * cur_func_node = program_tree->root;
    FrontEndErrors fe_errors = get_function(tokens, name_table, token_index,
                                            program_tree, cur_func_node);
    if (cur_func_node->right) cur_func_node = cur_func_node->right;

    while (is_function(tokens + *token_index))
    {
        (*token_index)++;
        fe_errors |= get_function(tokens, name_table, token_index,
                                  program_tree, cur_func_node);
        if (cur_func_node->right) cur_func_node = cur_func_node->right;
    }

    if (program_tree->root->value.type != TOKEN_TYPES_NAME_TABLE_ELEM)
        return FRONT_END_ERRORS_NO_MAIN_FUNCTION;

    return fe_errors;
}


static bool is_function(LangToken * token)
{
    MY_ASSERT(token);

    return token->type == TOKEN_TYPES_KEY_WORD &&
           token->val.kwd_id == KEY_WORDS_FUNCTION_START;
}


static FrontEndErrors get_function(LangToken * tokens, NameTable * name_table,
                                   size_t * token_index, Tree * program_tree,
                                   TreeNode * function_node)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(program_tree);
    MY_ASSERT(token_index);
    MY_ASSERT(function_node);

    FrontEndErrors fe_errors = 0;
    // if (!is_data_type(tokens + *token_index))
    //     return FRONT_END_ERRORS_DATA_TYPE_EXPECTED;
    // LangToken ret_data_type = tokens[(*token_index)++];

    if (tokens[*token_index].type != TOKEN_TYPES_NAME_TABLE_ELEM)
    {
        return fe_errors | FRONT_END_ERRORS_INVALID_FUNCTION_NAME;
    }

    TError_t tree_errors = 0;
    NameTableElemId nt_elem_id = tokens[*token_index].val.elem_id;
    if (name_table->names[nt_elem_id].length == START_FUNCTION_LEN &&
        !strncmp(name_table->names[nt_elem_id].name, START_FUNCTION_NAME,
                 START_FUNCTION_LEN))
    {
        function_node = program_tree->root;
    }
    else
    {
        TreeNode * * tmp_function_node = &(function_node->right);
        tree_errors |= tree_create_node(program_tree, function_node,
                                        tmp_function_node);
        function_node = *tmp_function_node;
    }
    if (tree_errors) return fe_errors | FRONT_END_ERRORS_TREE_ERROR;
    function_node->value = tokens[(*token_index)++];

    TreeNode * * func_def_node = &(function_node->left);
    if (tree_create_node(program_tree, function_node, func_def_node))
        return fe_errors | FRONT_END_ERRORS_TREE_ERROR;
    (*func_def_node)->value.type = TOKEN_TYPES_KEY_WORD;
    (*func_def_node)->value.val.kwd_id = KEY_WORDS_FUNCTION_START;

    bool defined_vars[NAME_TABLE_CAPACITY] = {};
    TreeNode * * args_node = &((*func_def_node)->left);
    if (tree_create_node(program_tree, *func_def_node, args_node))
        return fe_errors | FRONT_END_ERRORS_TREE_ERROR;
    fe_errors |= get_func_args(tokens, name_table, token_index,
                               program_tree, *args_node, defined_vars);
    if (fe_errors) return fe_errors;

    TreeNode * * body_node = &((*func_def_node)->right);
    if (tree_create_node(program_tree, *func_def_node, body_node))
        return fe_errors | FRONT_END_ERRORS_TREE_ERROR;
    fe_errors |= get_func_body(tokens, name_table, token_index,
                               program_tree, *body_node, defined_vars);

    return fe_errors;
}


static bool is_data_type(LangToken * token)
{
    MY_ASSERT(token);

    return token->type == TOKEN_TYPES_KEY_WORD &&
          (token->val.kwd_id == KEY_WORDS_BOOL ||
           token->val.kwd_id == KEY_WORDS_VOID ||
           token->val.kwd_id == KEY_WORDS_CHAR ||
           token->val.kwd_id == KEY_WORDS_INT  ||
           token->val.kwd_id == KEY_WORDS_DOUBLE);
}


static FrontEndErrors get_func_args(LangToken * tokens, NameTable * name_table,
                                    size_t * token_index, Tree * program_tree,
                                    TreeNode * arg_node, bool * defined_vars)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);
    MY_ASSERT(program_tree);
    MY_ASSERT(arg_node);

    if (tokens[*token_index].type != TOKEN_TYPES_SYMBOL ||
        tokens[*token_index].val.sym != '(')
    {
        return FRONT_END_ERRORS_NO_OPEN_BRAKET;
    }
    (*token_index)++;

    // --------------------------------------------------------------
    FrontEndErrors fe_errors = get_def_arg(tokens, name_table, token_index,
                                           program_tree, arg_node, defined_vars);
    // ----------------------------------------------------------------------------

    if (tokens[*token_index].type != TOKEN_TYPES_SYMBOL ||
        tokens[*token_index].val.sym != ')')
    {
        fe_errors |= FRONT_END_ERRORS_NO_CLOSE_BRAKET;
    }
    (*token_index)++;

    return fe_errors;
}


static FrontEndErrors get_def_arg(LangToken * tokens, NameTable * name_table,
                                  size_t * token_index, Tree * program_tree,
                                  TreeNode * arg_node, bool * defined_vars)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);
    MY_ASSERT(program_tree);
    MY_ASSERT(arg_node);

    if (!is_data_type(tokens + *token_index))
        return FRONT_END_ERRORS_DATA_TYPE_EXPECTED;
    arg_node->value = tokens[*token_index];
    (*token_index)++;

    FrontEndErrors fe_errors = 0;
    if (tokens[*token_index].type == TOKEN_TYPES_NAME_TABLE_ELEM)
    {
        TreeNode * * arg_name_node = &(arg_node->right);
        if (tree_create_node(program_tree, arg_node, arg_name_node))
            return fe_errors | FRONT_END_ERRORS_TREE_ERROR;
        (*arg_name_node)->value = tokens[*token_index];
        size_t nt_elem_id = tokens[*token_index].val.elem_id;
        defined_vars[nt_elem_id] = true;
        (*token_index)++;
    }

    if (tokens[*token_index].type == TOKEN_TYPES_SYMBOL &&
        tokens[*token_index].val.sym == ',')
    {
        (*token_index)++;
        TreeNode * * new_arg_node = &(arg_node->left);
        if (tree_create_node(program_tree, arg_node, new_arg_node))
            return fe_errors | FRONT_END_ERRORS_TREE_ERROR;
        fe_errors |= get_def_arg(tokens, name_table, token_index,
                                 program_tree, *new_arg_node, defined_vars);
    }

    return fe_errors;
}


static FrontEndErrors get_func_body(LangToken * tokens, NameTable * name_table,
                                    size_t * token_index, Tree * program_tree,
                                    TreeNode * body_node, bool * defined_vars)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);
    MY_ASSERT(program_tree);
    MY_ASSERT(body_node);
    MY_ASSERT(defined_vars);

    if (tokens[*token_index].type != TOKEN_TYPES_KEY_WORD ||
        tokens[*token_index].val.sym != KEY_WORDS_BODY_START)
    {
        return FRONT_END_ERRORS_INVALID_BODY_CONSTRUCTION;
    }
    (*token_index)++;

    TreeNode * cur_node = body_node;
    bool defined_local_vars[NAME_TABLE_CAPACITY] = {};
    for (size_t i = 0; i < NAME_TABLE_CAPACITY; i++)
        defined_local_vars[i] = defined_vars[i];

    FrontEndErrors fe_errors = 0;
    bool is_op = true;
    while (is_op)
    {
        TreeNode * * next_op_node = &(cur_node->right);
        
        if (is_if(tokens + *token_index))
        {
            fe_errors |= get_if(tokens, name_table, token_index,
                                program_tree, cur_node, defined_local_vars);

            if (tree_create_node(program_tree, cur_node, next_op_node))
                fe_errors |= FRONT_END_ERRORS_TREE_ERROR;
            cur_node = *next_op_node;

            if (fe_errors) return fe_errors;
            continue;
        }
        else if (is_while(tokens + *token_index))
        {
            fe_errors |= get_while(tokens, name_table, token_index,
                                   program_tree, cur_node, defined_local_vars);

            if (tree_create_node(program_tree, cur_node, next_op_node))
                fe_errors |= FRONT_END_ERRORS_TREE_ERROR;
            cur_node = *next_op_node;

            if (fe_errors) return fe_errors;
            continue;
        }
        else if (is_ret(tokens + *token_index))
        {
            fe_errors |= get_ret(tokens, name_table, token_index,
                                 program_tree, cur_node, defined_local_vars);

            if (tree_create_node(program_tree, cur_node, next_op_node))
                fe_errors |= FRONT_END_ERRORS_TREE_ERROR;
            cur_node = *next_op_node;

            if (fe_errors) return fe_errors;
            continue;
        }
        else if (is_calc_op(tokens, *token_index))
        {
            fe_errors |= get_calc_op(tokens, name_table, token_index,
                                     program_tree, cur_node, defined_local_vars);

            if (tree_create_node(program_tree, cur_node, next_op_node))
                fe_errors |= FRONT_END_ERRORS_TREE_ERROR;
            cur_node = *next_op_node;

            if (fe_errors) return fe_errors;
            continue;
        }

        is_op = false;
    }
    cur_node->value.type = TOKEN_TYPES_OPERATOR;
    cur_node->value.val.op_id = OPERATORS_END_OF_OP; 

    if (tokens[*token_index].type != TOKEN_TYPES_KEY_WORD ||
        tokens[*token_index].val.sym != KEY_WORDS_BODY_END)
    {
        fe_errors |= FRONT_END_ERRORS_INVALID_BODY_CONSTRUCTION;
    }
    (*token_index)++;

    return fe_errors;
}


static FrontEndErrors get_ret(LangToken * tokens, NameTable * name_table,
                              size_t * token_index, Tree * program_tree,
                              TreeNode * node, bool * defined_vars)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(defined_vars);

    node->value.type = TOKEN_TYPES_OPERATOR;
    node->value.val.op_id  = OPERATORS_END_OF_OP;

    TreeNode * * ret_node = &(node->left);
    if (tree_create_node(program_tree, node, ret_node))
        return FRONT_END_ERRORS_TREE_ERROR;
    (*ret_node)->value = tokens[(*token_index)++];

    TreeNode * * ret_val_node = &((*ret_node)->left);
    if (tree_create_node(program_tree, *ret_node, ret_val_node))
        return FRONT_END_ERRORS_TREE_ERROR;
    FrontEndErrors fe_errors = get_expression(tokens, name_table,
                                              token_index, program_tree,
                                              *ret_val_node, defined_vars);
    if (fe_errors) return fe_errors;

    if (tokens[*token_index].type != TOKEN_TYPES_OPERATOR ||
        tokens[*token_index].val.op_id != OPERATORS_END_OF_OP)
        return FRONT_END_ERRORS_END_OF_OP_EXPECTED;

    (*token_index)++;
    return fe_errors;
}


static FrontEndErrors get_calc_op(LangToken * tokens, NameTable * name_table,
                                  size_t * token_index, Tree * program_tree,
                                  TreeNode * node, bool * defined_vars)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(defined_vars);

    node->value.type = TOKEN_TYPES_OPERATOR;
    node->value.val.op_id = OPERATORS_END_OF_OP;

    TreeNode * * calc_node = &(node->left);
    if (tree_create_node(program_tree, node, calc_node))
        return FRONT_END_ERRORS_TREE_ERROR;

    FrontEndErrors fe_errors = 0;
    if (is_data_type(tokens + *token_index))
    {
        fe_errors |= get_definition(tokens, name_table, token_index,
                                    program_tree, *calc_node, defined_vars);
    }
    else
    {
        fe_errors |= get_expression(tokens, name_table, token_index,
                                    program_tree, *calc_node, defined_vars);
    }

    if (tokens[*token_index].type != TOKEN_TYPES_OPERATOR ||
        tokens[*token_index].val.op_id != OPERATORS_END_OF_OP)
        fe_errors |= FRONT_END_ERRORS_END_OF_OP_EXPECTED;
    (*token_index)++;

    return fe_errors;

    #if 0
        1. Dobavit funkcii v get_expression();      -> DONE (vrode bi -_-)
        2. get_definition;                          -> DONE
        3. check na ";" v get_calc_op();            -> DONE

        4. Pomenyat kwd i op na kwd_id i op_id      -> POKA V PADLY
        5. Kringe structures names                  
        6. Poyzhe ekran                             -> DONE
        7. Obyavleniya funkciy                      -> POHUI
        8. Kringe no const { } ( ) ,                -> DONE HZ
    #endif
}


static FrontEndErrors get_definition(LangToken * tokens, NameTable * name_table,
                                     size_t * token_index, Tree * program_tree,
                                     TreeNode * node, bool * defined_vars)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(defined_vars);

    node->value = tokens[(*token_index)++];

    NameTableElemId nt_elem_id = tokens[*token_index].val.elem_id;
    if (tokens[*token_index].type != TOKEN_TYPES_NAME_TABLE_ELEM ||
        defined_vars[nt_elem_id])
        return FRONT_END_ERRORS_INVALID_VAR_NAME;

    TreeNode * * name_node = &(node->left);
    if (tree_create_node(program_tree, node, name_node))
        return FRONT_END_ERRORS_TREE_ERROR;
    (*name_node)->value = tokens[*token_index];
    defined_vars[nt_elem_id] = true;
    (*token_index)++;

    if (tokens[*token_index].type != TOKEN_TYPES_OPERATOR ||
        tokens[*token_index].val.op_id != OPERATORS_ASSIGMENT)
    {
        return FRONT_END_ERRORS_DEFINITION_EXPECTED;
    }
    (*token_index)++;

    TreeNode * * val_node = &(node->right);
    if (tree_create_node(program_tree, node, val_node))
        return FRONT_END_ERRORS_TREE_ERROR;
    return get_expression(tokens, name_table, token_index,
                          program_tree, *val_node, defined_vars);
}


static FrontEndErrors get_if(LangToken * tokens, NameTable * name_table,
                             size_t * token_index, Tree * program_tree,
                             TreeNode * cur_node, bool * defined_vars)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);
    MY_ASSERT(program_tree);
    MY_ASSERT(cur_node);
    MY_ASSERT(defined_vars);

    cur_node->value.type = TOKEN_TYPES_OPERATOR;
    cur_node->value.val.op_id  = OPERATORS_END_OF_OP;

    TreeNode * * if_node = &(cur_node->left);
    if (tree_create_node(program_tree, cur_node, if_node))
        return FRONT_END_ERRORS_TREE_ERROR;
    (*if_node)->value = tokens[(*token_index)++];

    TreeNode * * condition_node = &((*if_node)->left);
    if (tree_create_node(program_tree, *if_node, condition_node))
        return FRONT_END_ERRORS_TREE_ERROR;

    if (tokens[*token_index].type != TOKEN_TYPES_SYMBOL ||
        tokens[*token_index].val.sym != '(')
        return FRONT_END_ERRORS_NO_OPEN_BRAKET;
    (*token_index)++;
    FrontEndErrors fe_errors = get_expression(tokens, name_table,
                                              token_index, program_tree,
                                              *condition_node, defined_vars);
    if (tokens[*token_index].type != TOKEN_TYPES_SYMBOL ||
        tokens[*token_index].val.sym != ')')
        return FRONT_END_ERRORS_NO_OPEN_BRAKET;
    (*token_index)++;

    TreeNode * * body_node = &((*if_node)->right);
    if (tree_create_node(program_tree, *if_node, body_node))
        return FRONT_END_ERRORS_TREE_ERROR;
    fe_errors |= get_func_body(tokens, name_table, token_index,
                                program_tree, *body_node, defined_vars);

    return fe_errors;
}


static FrontEndErrors get_while(LangToken * tokens, NameTable * name_table,
                                size_t * token_index, Tree * program_tree,
                                TreeNode * cur_node, bool * defined_vars)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);
    MY_ASSERT(program_tree);
    MY_ASSERT(cur_node);
    MY_ASSERT(defined_vars);

    cur_node->value.type = TOKEN_TYPES_OPERATOR;
    cur_node->value.val.op_id  = OPERATORS_END_OF_OP;

    TreeNode * * while_node = &(cur_node->left);
    if (tree_create_node(program_tree, cur_node, while_node))
        return FRONT_END_ERRORS_TREE_ERROR;
    (*while_node)->value = tokens[(*token_index)++];

    TreeNode * * condition_node = &((*while_node)->left);
    if (tree_create_node(program_tree, *while_node, condition_node))
        return FRONT_END_ERRORS_TREE_ERROR;

    if (tokens[*token_index].type != TOKEN_TYPES_SYMBOL ||
        tokens[*token_index].val.sym != '(')
        return FRONT_END_ERRORS_NO_OPEN_BRAKET;
    (*token_index)++;
    FrontEndErrors fe_errors = get_expression(tokens, name_table,
                                              token_index, program_tree,
                                              *condition_node, defined_vars);
    if (tokens[*token_index].type != TOKEN_TYPES_SYMBOL ||
        tokens[*token_index].val.sym != ')')
        return FRONT_END_ERRORS_NO_OPEN_BRAKET;
    (*token_index)++;

    TreeNode * * body_node = &((*while_node)->right);
    if (tree_create_node(program_tree, *while_node, body_node))
        return FRONT_END_ERRORS_TREE_ERROR;
    fe_errors |= get_func_body(tokens, name_table, token_index,
                                program_tree, *body_node, defined_vars);

    return fe_errors;
}


static FrontEndErrors get_expression(LangToken * tokens, NameTable * name_table,
                                     size_t * token_index, Tree * program_tree,
                                     TreeNode * exp_node, bool * defined_vars)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);
    MY_ASSERT(program_tree);
    MY_ASSERT(exp_node);
    MY_ASSERT(defined_vars);

    TreeNode * left_part_node = NULL;
    if (tree_create_node(program_tree, NULL, &left_part_node))
        return FRONT_END_ERRORS_TREE_ERROR;
    FrontEndErrors fe_errors = get_e(tokens, name_table, token_index,
                                     program_tree, left_part_node, defined_vars);

    if (is_comprassion_op(tokens + *token_index))
    {
        exp_node->value = tokens[(*token_index)++];
        exp_node->left = left_part_node;
        left_part_node->parent = exp_node;

        TreeNode * * right_part_node = &(exp_node->right);
        if (tree_create_node(program_tree, exp_node, right_part_node))
            return fe_errors | FRONT_END_ERRORS_TREE_ERROR;
        fe_errors |= get_e(tokens, name_table, token_index,
                           program_tree, *right_part_node, defined_vars);
    }
    else
    {
        exp_node->value = left_part_node->value;
        exp_node->right = left_part_node->right;
        exp_node->left  = left_part_node->left;
        free(left_part_node);
    }

    return fe_errors;
}


static FrontEndErrors get_e(LangToken * tokens, NameTable * name_table,
                            size_t * token_index, Tree * program_tree,
                            TreeNode * node, bool * defined_vars)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(defined_vars);

    TreeNode * left_node = NULL;
    if (tree_create_node(program_tree, NULL, &left_node))
        return FRONT_END_ERRORS_TREE_ERROR;
    FrontEndErrors fe_errors = get_tp(tokens, name_table, token_index,
                                      program_tree, left_node, defined_vars);

    bool no_e_op = true;
    while (is_e_op(tokens + *token_index))
    {
        node->value = tokens[(*token_index)++];
        node->left = left_node;
        left_node->parent = node;

        TreeNode * * right_node = &(node->right);
        if (tree_create_node(program_tree, node, right_node))
            return fe_errors | FRONT_END_ERRORS_TREE_ERROR;
        fe_errors |= get_e(tokens, name_table, token_index,
                           program_tree, *right_node, defined_vars);

        no_e_op = false;
    }

    if (no_e_op)
    {
        node->value = left_node->value;
        node->right = left_node->right;
        node->left  = left_node->left;
        free(left_node);
    }

    return fe_errors;
}


static FrontEndErrors get_tp(LangToken * tokens, NameTable * name_table,
                             size_t * token_index, Tree * program_tree,
                             TreeNode * node, bool * defined_vars)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(defined_vars);

    TreeNode * left_node = NULL;
    if (tree_create_node(program_tree, NULL, &left_node))
        return FRONT_END_ERRORS_TREE_ERROR;
    FrontEndErrors fe_errors = get_p(tokens, name_table, token_index,
                                     program_tree, left_node, defined_vars);

    bool no_tp_op = true;
    while (is_tp_op(tokens + *token_index))
    {
        node->value = tokens[(*token_index)++];
        node->left = left_node;
        left_node->parent = node;

        TreeNode * * right_node = &(node->right);
        if (tree_create_node(program_tree, node, right_node))
            return fe_errors | FRONT_END_ERRORS_TREE_ERROR;
        fe_errors |= get_tp(tokens, name_table, token_index,
                           program_tree, *right_node, defined_vars);

        no_tp_op = false;
    }

    if (no_tp_op)
    {
        node->value = left_node->value;
        node->right = left_node->right;
        node->left  = left_node->left;
        free(left_node);
    }

    return fe_errors;
}


static FrontEndErrors get_p(LangToken * tokens, NameTable * name_table,
                            size_t * token_index, Tree * program_tree,
                            TreeNode * node, bool * defined_vars)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(defined_vars);

    if (tokens[*token_index].type == TOKEN_TYPES_SYMBOL &&
        tokens[*token_index].val.sym == '(')
    {
        (*token_index)++;
        FrontEndErrors fe_errors = get_e(tokens, name_table, token_index,
                                         program_tree, node, defined_vars);

        if (tokens[*token_index].type != TOKEN_TYPES_SYMBOL ||
            tokens[*token_index].val.sym != ')')
            fe_errors |= FRONT_END_ERRORS_NO_CLOSE_BRAKET;

        (*token_index)++;
        return fe_errors;
    }

    return get_n(tokens, name_table, token_index,
                 program_tree, node, defined_vars);
}


static FrontEndErrors get_n(LangToken * tokens, NameTable * name_table,
                            size_t * token_index, Tree * program_tree,
                            TreeNode * node, bool * defined_vars)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(defined_vars);

    if (tokens[*token_index].type == TOKEN_TYPES_NUMBER)
    {
        node->value = tokens[(*token_index)++];
        return 0;
    }
    else if (is_func_call(tokens,  *token_index))
    {
        return get_func_call(tokens, name_table, token_index,
                             program_tree, node, defined_vars);
    }
    else if (tokens[*token_index].type == TOKEN_TYPES_NAME_TABLE_ELEM)
    {
        node->value = tokens[(*token_index)++];
        NameTableElemId nt_elem_id = node->value.val.elem_id;

        if (!defined_vars[nt_elem_id])
            return FRONT_END_ERRORS_UNDEFINED_VAR;

        return 0;
    }
    else if (is_in(tokens + *token_index))
    {
        node->value = tokens[(*token_index)++];
        return 0;
    }
    else if (is_out(tokens + *token_index))
    {
        return get_out(tokens, name_table, token_index,
                       program_tree, node, defined_vars);
    }
    else if (is_sqrt(tokens + *token_index))
    {
        return get_sqrt(tokens, name_table, token_index,
                        program_tree, node, defined_vars);
    }

    return FRONT_END_ERRORS_EXPRESSION_EXPECTED;
}


static FrontEndErrors get_sqrt(LangToken * tokens, NameTable * name_table,
                               size_t * token_index, Tree * program_tree,
                               TreeNode * node, bool * defined_vars)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(defined_vars);

    node->value = tokens[(*token_index)++];

    if (tokens[*token_index].type != TOKEN_TYPES_SYMBOL ||
        tokens[*token_index].val.sym != '(')
        return FRONT_END_ERRORS_NO_OPEN_BRAKET;
    (*token_index)++;

    TreeNode * * out_val_node = &(node->left);
    if (tree_create_node(program_tree, node, out_val_node))
        return FRONT_END_ERRORS_TREE_ERROR;
    
    FrontEndErrors fe_errors =  get_expression(tokens, name_table, token_index,
                                               program_tree, *out_val_node, defined_vars);
    if (fe_errors) return fe_errors;

    if (tokens[*token_index].type != TOKEN_TYPES_SYMBOL ||
        tokens[*token_index].val.sym != ')')
        return FRONT_END_ERRORS_NO_CLOSE_BRAKET;
    (*token_index)++;

    return fe_errors;
}


static FrontEndErrors get_out(LangToken * tokens, NameTable * name_table,
                              size_t * token_index, Tree * program_tree,
                              TreeNode * node, bool * defined_vars)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(defined_vars);

    node->value = tokens[(*token_index)++];

    TreeNode * * out_val_node = &(node->left);
    if (tree_create_node(program_tree, node, out_val_node))
        return FRONT_END_ERRORS_TREE_ERROR;
    
    return get_expression(tokens, name_table, token_index,
                          program_tree, *out_val_node, defined_vars);
}


static FrontEndErrors get_func_call(LangToken * tokens, NameTable * name_table,
                                    size_t * token_index, Tree * program_tree,
                                    TreeNode * node, bool * defined_vars)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(defined_vars);

    node->value.type = TOKEN_TYPES_KEY_WORD;
    node->value.val.kwd_id = KEY_WORDS_FUNCTION_START;

    TreeNode * * function_name = &(node->left);
    if (tree_create_node(program_tree, node, function_name))
        return FRONT_END_ERRORS_TREE_ERROR;
    (*function_name)->value = tokens[(*token_index)++];

    (*token_index)++;
    FrontEndErrors fe_errors = 0;
    if (is_func_call_arg(tokens + *token_index))
    {
        TreeNode * * func_call_args = &(node->right);
        if (tree_create_node(program_tree, node, func_call_args))
            return FRONT_END_ERRORS_TREE_ERROR;
        fe_errors |= get_func_call_arg(tokens, name_table, token_index,
                                       program_tree, *func_call_args, defined_vars);

        while (tokens[*token_index].type == TOKEN_TYPES_SYMBOL &&
               tokens[*token_index].val.sym == ',')
        {
            (*token_index)++;
            if (!is_func_call_arg(tokens + *token_index))
                return fe_errors | FRONT_END_ERRORS_INVALID_ARGUMENT;

            TreeNode * * cur_node = &((*func_call_args)->right);
            if (tree_create_node(program_tree, *func_call_args, cur_node))
                return FRONT_END_ERRORS_TREE_ERROR;
            fe_errors |= get_func_call_arg(tokens, name_table, token_index,
                                           program_tree, *cur_node, defined_vars);
            func_call_args = cur_node;
        }
    }

    if (tokens[*token_index].type != TOKEN_TYPES_SYMBOL ||
        tokens[*token_index].val.sym != ')')
        fe_errors |= FRONT_END_ERRORS_NO_CLOSE_BRAKET;
    (*token_index)++;

    return fe_errors;
}


static FrontEndErrors get_func_call_arg(LangToken * tokens, NameTable * name_table,
                                        size_t * token_index, Tree * program_tree,
                                        TreeNode * node, bool * defined_vars)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);
    MY_ASSERT(token_index);
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(defined_vars);

    node->value.type = TOKEN_TYPES_SYMBOL;
    node->value.val.sym = ',';

    TreeNode * * arg_exp = &(node->left);
    if (tree_create_node(program_tree, node, arg_exp))
        return FRONT_END_ERRORS_TREE_ERROR;
    return get_expression(tokens, name_table, token_index,
                          program_tree, *arg_exp, defined_vars);
}



static bool is_func_call_arg(LangToken * token)
{
    MY_ASSERT(token);

    return token->type == TOKEN_TYPES_NUMBER ||
           token->type == TOKEN_TYPES_NAME_TABLE_ELEM;
}


static bool is_func_call(LangToken * tokens, size_t token_index)
{
    MY_ASSERT(tokens);

    return tokens[token_index + 1].type == TOKEN_TYPES_SYMBOL &&
           tokens[token_index + 1].val.sym == '(' &&
           tokens[token_index].type == TOKEN_TYPES_NAME_TABLE_ELEM;
}


static bool is_e_op(LangToken * token)
{
    MY_ASSERT(token);

    return token->type == TOKEN_TYPES_OPERATOR &&
          (token->val.op_id == OPERATORS_ADDITION ||
           token->val.op_id == OPERATORS_SUBTRACTION);
}


static bool is_tp_op(LangToken * token)
{
    MY_ASSERT(token);

    return token->type == TOKEN_TYPES_OPERATOR &&
          (token->val.op_id == OPERATORS_MULTIPLICATION ||
           token->val.op_id == OPERATORS_DIVISION);
}


static bool is_calc_op(LangToken * tokens, size_t token_index)
{
    MY_ASSERT(tokens);
    MY_ASSERT(token_index);

    while ((tokens[token_index].type != TOKEN_TYPES_KEY_WORD ||
           tokens[token_index].val.sym != KEY_WORDS_BODY_END) &&
           tokens[token_index].type != TOKEN_TYPES_TERMINATOR)
    {
        if (tokens[token_index].type == TOKEN_TYPES_OPERATOR &&
            tokens[token_index].val.op_id == OPERATORS_END_OF_OP)
        {
            return true;
        }
        token_index++;
    }

    return false;
}


static bool is_if(LangToken * token)
{
    MY_ASSERT(token);

    return token->type == TOKEN_TYPES_KEY_WORD &&
           token->val.kwd_id == KEY_WORDS_IF;
}


static bool is_in(LangToken * token)
{
    MY_ASSERT(token);

    return token->type == TOKEN_TYPES_KEY_WORD &&
           token->val.kwd_id == KEY_WORDS_IN;
}


static bool is_out(LangToken * token)
{
    MY_ASSERT(token);

    return token->type == TOKEN_TYPES_KEY_WORD &&
           token->val.kwd_id == KEY_WORDS_OUT;
}


static bool is_sqrt(LangToken * token)
{
    MY_ASSERT(token);

    return token->type == TOKEN_TYPES_OPERATOR &&
           token->val.op_id == OPERATORS_SQUARE_ROOT;
}


static bool is_while(LangToken * token)
{
    MY_ASSERT(token);

    return token->type == TOKEN_TYPES_KEY_WORD &&
           token->val.kwd_id == KEY_WORDS_WHILE;
}


static bool is_ret(LangToken * token)
{
    MY_ASSERT(token);

    return token->type == TOKEN_TYPES_KEY_WORD &&
           token->val.kwd_id == KEY_WORDS_RETURN;
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
        if (strncmp(OPERATORS_ARRAY[i].value, buffer,
                    OPERATORS_ARRAY[i].length) == 0)
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


static bool is_spec_symbol(char symbol)
{
    return is_braket(symbol) || (symbol == ',');
}


static bool is_comprassion_op(LangToken * token)
{
    MY_ASSERT(token);

    return token->type == TOKEN_TYPES_OPERATOR &&
          (token->val.op_id == OPERATORS_BELOW ||
           token->val.op_id == OPERATORS_ABOVE ||
           token->val.op_id == OPERATORS_EQUAL ||
           token->val.op_id == OPERATORS_ASSIGMENT);
}


void tokens_dump(const LangToken * tokens, NameTable * name_table)
{
    MY_ASSERT(tokens);
    MY_ASSERT(name_table);

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
                printf("[%zd]\t %-25s | OPERATOR\n", i, OPERATORS_ARRAY[tokens[i].val.op_id].value);
                break;

            case TOKEN_TYPES_KEY_WORD:
                printf("[%zd]\t %-25s | KEY WORD\n", i, KEY_WORDS_ARRAY[tokens[i].val.kwd_id].name);
                break;

            case TOKEN_TYPES_SYMBOL:
                printf("[%zd]\t %-25c | SYMBOL\n", i, tokens[i].val.sym);
                break;

            case TOKEN_TYPES_NAME_TABLE_ELEM:
                printf("[%zd]\t %-25s | NAME TABLE ELEMENT\n", i,
                       name_table->names[tokens[i].val.elem_id].name);
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


void fe_error_output(FrontEndErrors fe_errors, size_t token_index)
{
    printf("!!!Syntaxis Error!!!\n");
    #define ERROR_PROC(error_id, error_message)                     \
        if (fe_errors & error_id)                                   \
            printf("Token %zd: %s\n", token_index, error_message);

    #include "front_end_errors.h"

    #undef ERROR_PROC

    puts("");
    return;
}

#ifndef NDEBUG
    static void tok_dump(LangToken * token)
    {
        MY_ASSERT(token);

        printf("Cur token: ");
        switch (token->type)
        {
            case TOKEN_TYPES_KEY_WORD:
                printf("%s (kwd)\n", KEY_WORDS_ARRAY[token->val.kwd_id].name);
                break;
            case TOKEN_TYPES_NAME_TABLE_ELEM:
                printf("%zd (nt elem id)\n", token->val.elem_id);
                break;
            case TOKEN_TYPES_NUMBER:
                printf("%.2lf (num)\n", token->val.num);
                break;
            case TOKEN_TYPES_OPERATOR:
                printf("%s (op)\n", OPERATORS_ARRAY[token->val.op_id].value);
                break;
            case TOKEN_TYPES_SYMBOL:
                printf("%c (sym)\n", token->val.sym);
                break;
            case TOKEN_TYPES_TERMINATOR:
                printf("\\0 (terminator)\n");
                break;
            default:
                break;
        }

        return;
    }
#endif // NDEBUG