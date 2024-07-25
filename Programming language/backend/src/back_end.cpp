#include <stdio.h>
#include <stdlib.h>

#include "back_end.h"
#include "my_assert.h"


const char * ASM_CODE_FILE = "beprog.sasm";


static void print_procedure(FILE * fp, TreeNode * proc_node, size_t * loops_cnt);
static void print_args_processing(FILE * fp, TreeNode * args_node);
static void print_body(FILE * fp, TreeNode * body_node, size_t * loops_cnt);
static void print_operator(FILE * fp, TreeNode * op_node, size_t * loops_cnt);
static void print_kwd(FILE * fp, TreeNode * kwd_node, size_t * loops_cnt);
static void print_op(FILE * fp, TreeNode * op_node, size_t * loops_cnt);
static void print_pow(FILE * fp, size_t * loops_cnt);
static void print_nt_elem(FILE * fp, TreeNode * nt_elem_node);
static void print_num(FILE * fp, TreeNode * num_node);


void print_asm_code(FILE * fp, Tree * prog_tree)
{
    MY_ASSERT(fp);
    MY_ASSERT(prog_tree);
    MY_ASSERT(prog_tree->root->value.type == TOKEN_TYPES_NAME_TABLE_ELEM);

    fprintf(fp, "call :proc%zd\n"
                "hlt\n\n",
                prog_tree->root->value.val.elem_id);

    size_t loops_cnt = 0;
    print_procedure(fp, prog_tree->root, &loops_cnt);
    
    return;
}


static void print_procedure(FILE * fp, TreeNode * proc_node, size_t * loops_cnt)
{
    MY_ASSERT(fp);
    MY_ASSERT(proc_node);
    MY_ASSERT(loops_cnt);
    MY_ASSERT(proc_node->value.type == TOKEN_TYPES_NAME_TABLE_ELEM);

    fprintf(fp, ";------------------------------------------\n"
                ":proc%zd\n", 
                proc_node->value.val.elem_id);

    TreeNode * args_node = proc_node->left->left;
    print_args_processing(fp, args_node);
    
    TreeNode * body_node = proc_node->left->right;
    print_body(fp, body_node, loops_cnt);
    fprintf(fp, ";------------------------------------------\n\n");

    TreeNode * next_proc_node = proc_node->right;
    if (next_proc_node)
        print_procedure(fp, next_proc_node, loops_cnt);

    return;
}

static void print_args_processing(FILE * fp, TreeNode * args_node)
{
    MY_ASSERT(fp);
    MY_ASSERT(args_node);
    MY_ASSERT(args_node->value.type == TOKEN_TYPES_KEY_WORD);

    if (args_node->value.val.kwd_id == KEY_WORDS_VOID)
        return;

    #if 0
        todo : vars size processing
    #endif 

    TreeNode * next_arg_node = args_node->left;
    if (next_arg_node) print_args_processing(fp, next_arg_node);

    TreeNode * arg_name_node = args_node->right;
    fprintf(fp, "\tpop [%zd]\n", arg_name_node->value.val.elem_id);

    return;
}


static void print_body(FILE * fp, TreeNode * body_node, size_t * loops_cnt)
{
    MY_ASSERT(fp);
    MY_ASSERT(body_node);
    MY_ASSERT(loops_cnt);
    MY_ASSERT(body_node->value.type == TOKEN_TYPES_OPERATOR);
    MY_ASSERT(body_node->value.val.op_id == OPERATORS_END_OF_OP);

    if (!body_node->right)
        return;

    print_operator(fp, body_node->left, loops_cnt);
    print_body(fp, body_node->right, loops_cnt);

    return;
}


static void print_operator(FILE * fp, TreeNode * op_node, size_t * loops_cnt)
{
    MY_ASSERT(fp);
    MY_ASSERT(op_node);
    MY_ASSERT(loops_cnt);

    switch (op_node->value.type)
    {
        case TOKEN_TYPES_KEY_WORD:
            print_kwd(fp, op_node, loops_cnt);
            break;
        
        case TOKEN_TYPES_NAME_TABLE_ELEM:
            print_nt_elem(fp, op_node);
            break;

        case TOKEN_TYPES_OPERATOR:
            print_op(fp, op_node, loops_cnt);
            break;

        case TOKEN_TYPES_NUMBER:
            print_num(fp, op_node);
            break;

        case TOKEN_TYPES_SYMBOL:
        case TOKEN_TYPES_TERMINATOR:
        default:
            MY_ASSERT(0 && "UNREACHABLE");
            break;
    }

    return;
}


static void print_kwd(FILE * fp, TreeNode * kwd_node, size_t * loops_cnt)
{
    MY_ASSERT(fp);
    MY_ASSERT(kwd_node);
    MY_ASSERT(loops_cnt);
    MY_ASSERT(kwd_node->value.type == TOKEN_TYPES_KEY_WORD);

    switch (kwd_node->value.val.kwd_id)
    {
        #if 0
            todo : variables size processing
        #endif 

        case KEY_WORDS_BOOL:
        case KEY_WORDS_CHAR:
        case KEY_WORDS_DOUBLE:
        case KEY_WORDS_INT:
        {
            fprintf(fp, ";-declaration-\n");
            TreeNode * var_node = kwd_node->left;
            MY_ASSERT(var_node->value.type == TOKEN_TYPES_NAME_TABLE_ELEM);
            TreeNode * var_val_node = kwd_node->right;
            print_operator(fp, var_val_node, loops_cnt);
            fprintf(fp, "\tpop [%zd]\n"
                        ";------------\n", 
                        var_node->value.val.elem_id);
            break;
        }

        case KEY_WORDS_FUNCTION_START:
        {
            fprintf(fp, ";-function call-\n");
            MY_ASSERT(kwd_node->right->value.type == TOKEN_TYPES_SYMBOL);
            MY_ASSERT(kwd_node->right->value.val.sym == ',');
            TreeNode * arg_set_node = kwd_node->right;
            while (arg_set_node)
            {
                TreeNode * arg_node = arg_set_node->left;
                MY_ASSERT(arg_node);
                print_operator(fp, arg_node, loops_cnt);
                arg_set_node = arg_set_node->right;
            }
            TreeNode * func_name_node = kwd_node->left;
            MY_ASSERT(func_name_node->value.type == TOKEN_TYPES_NAME_TABLE_ELEM);
            fprintf(fp, "\tcall :proc%zd\n"
                        ";------------\n", 
                        func_name_node->value.val.elem_id);
            break;
        }

        case KEY_WORDS_IF:
        {
            fprintf(fp, ";-----if-----\n");
            size_t loop_id = (*loops_cnt)++;
            TreeNode * condition_node = kwd_node->left;
            print_operator(fp, condition_node, loops_cnt);
            fprintf(fp, "\tpush 0\n"
                        "\tje :fail%zd\n", 
                        loop_id);
            TreeNode * if_body_node = kwd_node->right;
            print_body(fp, if_body_node, loops_cnt);
            fprintf(fp, ":fail%zd\n"
                        ";------------\n", 
                        loop_id);
            break;
        }

        case KEY_WORDS_WHILE:
        {
            size_t loop_id = (*loops_cnt)++;
            fprintf(fp, ";---while----\n"
                        ":check%zd\n", 
                        loop_id);
            TreeNode * condition_node = kwd_node->left;
            print_operator(fp, condition_node, loops_cnt);
            fprintf(fp, "\tpush 0\n"
                        "\tje :fail%zd\n", 
                        loop_id);
            TreeNode * if_body_node = kwd_node->right;
            print_body(fp, if_body_node, loops_cnt);
            fprintf(fp, "\tjmp :check%zd\n"
                        ":fail%zd\n"
                        ";------------\n", 
                        loop_id, loop_id);
            break; 
        }

        case KEY_WORDS_RETURN:
        {
            fprintf(fp, ";---return---\n");
            TreeNode * ret_val_node = kwd_node->left;
            print_operator(fp, ret_val_node, loops_cnt);
            fprintf(fp, "\tret\n"
                        ";------------\n");
            break;
        }

        case KEY_WORDS_IN:
            fprintf(fp, "\tin\n");
            break;
        
        case KEY_WORDS_OUT:
        {
            TreeNode * out_val_node = kwd_node->left;
            print_operator(fp, out_val_node, loops_cnt);
            fprintf(fp, "\tout\n");
            break;
        }

        case KEY_WORDS_VOID:
        default:
            MY_ASSERT(0 && "UNREACHABLE");
            break;
    }

    return;
}


static void print_op(FILE * fp, TreeNode * op_node, size_t * loops_cnt)
{
    MY_ASSERT(fp);
    MY_ASSERT(op_node);
    MY_ASSERT(op_node->value.type == TOKEN_TYPES_OPERATOR);

    switch (op_node->value.val.op_id)
    {
        case OPERATORS_ADDITION:
            print_operator(fp, op_node->left, loops_cnt);
            print_operator(fp, op_node->right, loops_cnt);
            fprintf(fp, "\tadd\n");
            break;

        case OPERATORS_SUBTRACTION:
            print_operator(fp, op_node->left, loops_cnt);
            print_operator(fp, op_node->right, loops_cnt);
            fprintf(fp, "\tsub\n");
            break;

        case OPERATORS_MULTIPLICATION:
            print_operator(fp, op_node->left, loops_cnt);
            print_operator(fp, op_node->right, loops_cnt);
            fprintf(fp, "\tmul\n");
            break;

        case OPERATORS_DIVISION:
            print_operator(fp, op_node->left, loops_cnt);
            print_operator(fp, op_node->right, loops_cnt);
            fprintf(fp, "\tdiv\n");
            break;

        case OPERATORS_POWER:
            print_operator(fp, op_node->left, loops_cnt);
            print_operator(fp, op_node->right, loops_cnt);
            print_pow(fp, loops_cnt);
            break;

        case OPERATORS_SQUARE_ROOT:
            print_operator(fp, op_node->left, loops_cnt);
            fprintf(fp, "\tsqrt\n");
            break;

        case OPERATORS_ASSIGMENT:
        {
            TreeNode * var_val_node = op_node->right;
            print_operator(fp, var_val_node, loops_cnt);
            TreeNode * var_node = op_node->left;
            MY_ASSERT(var_node->value.type == TOKEN_TYPES_NAME_TABLE_ELEM);
            fprintf(fp, "\tpop [%zd]\n", var_node->value.val.elem_id);
            break;
        }

        case OPERATORS_END_OF_OP:
            print_body(fp, op_node, loops_cnt);
            break;
        
        case OPERATORS_BELOW:
        {
            print_operator(fp, op_node->left, loops_cnt);
            print_operator(fp, op_node->right, loops_cnt);
            size_t loop_id = (*loops_cnt)++;
            fprintf(fp, "\tjb :true%zd\n"
                        "\tpush 0\n"
                        "\tjmp :exit%zd\n"
                        ":true%zd\n"
                        "\tpush 1\n"
                        ":exit%zd\n",
                        loop_id, loop_id, loop_id, loop_id);
            break;
        }

        case OPERATORS_ABOVE:
        {
            print_operator(fp, op_node->left, loops_cnt);
            print_operator(fp, op_node->right, loops_cnt);
            size_t loop_id = (*loops_cnt)++;
            fprintf(fp, "\tja :true%zd\n"
                        "\tpush 0\n"
                        "\tjmp :exit%zd\n"
                        ":true%zd\n"
                        "\tpush 1\n"
                        ":exit%zd\n",
                        loop_id, loop_id, loop_id, loop_id);
            break;
        }

        case OPERATORS_EQUAL:
        {
            print_operator(fp, op_node->left, loops_cnt);
            print_operator(fp, op_node->right, loops_cnt);
            size_t loop_id = (*loops_cnt)++;
            fprintf(fp, "\tje :true%zd\n"
                        "\tpush 0\n"
                        "\tjmp :exit%zd\n"
                        ":true%zd\n"
                        "\tpush 1\n"
                        ":exit%zd\n",
                        loop_id, loop_id, loop_id, loop_id);
            break;
        }

        default:
            MY_ASSERT(0 && "UNREACHABLE");
            break;
    }
}


static void print_pow(FILE * fp, size_t * loops_cnt)
{
    MY_ASSERT(fp);
    MY_ASSERT(loops_cnt);

    size_t loop_id = (*loops_cnt)++;
    fprintf(fp, "\tpop rcx\n"
                "\tpop rdx\n"
                "\tpush 1\n"
                ":check%zd\n"
                "\tpush rcx\n"
                "\tpush 0\n"
                "\tje :fail%zd\n"
                "\tpush rdx\n"
                "\tmul\n"
                "\tpush rcx\n"
                "\tpush 1\n"
                "\tsub\n"
                "\tjmp :check%zd\n"
                ":fail%zd\n",
                loop_id, loop_id, loop_id, loop_id);
    return;
}


static void print_nt_elem(FILE * fp, TreeNode * nt_elem_node)
{
    MY_ASSERT(fp);
    MY_ASSERT(nt_elem_node);
    MY_ASSERT(nt_elem_node->value.type == TOKEN_TYPES_NAME_TABLE_ELEM);

    fprintf(fp, "\tpush [%zd]\n", nt_elem_node->value.val.elem_id);
    return;
}


static void print_num(FILE * fp, TreeNode * num_node)
{
    MY_ASSERT(fp);
    MY_ASSERT(num_node);
    MY_ASSERT(num_node->value.type == TOKEN_TYPES_NUMBER);

    fprintf(fp, "\tpush %.3lf\n", num_node->value.val.num);
    return;
}

//  ; if
//  push 1 ; cond
//  push 0
//  je :fail0
//  ; print body
//  :fail0

//  ; while
//  :check1
//  push 1 ; cond
//  push 0 
//  je :fail1
//  ; print body
//  jmp :check1
//  :fail1