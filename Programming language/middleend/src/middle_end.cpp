#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "middle_end.h"
#include "tree.h"
#include "double_comparing.h"
#include "my_assert.h"


static TError_t calculation_optimization(Tree * program_tree, TreeNode * node,
                                         bool * success);
static TError_t try_calculate_node(Tree * program_tree, TreeNode * node,
                                   bool * success);
static double calculate_new_value(Tree * program_tree, TreeNode * node);
static TError_t replace_optimization(Tree * program_tree, TreeNode * node, 
                                     bool * success);
static TError_t try_replace_node(Tree * program_tree, TreeNode * node,
                                 bool * success);
static TError_t try_replace_lzero(Tree * program_tree, TreeNode * node,
                                  bool * success);
static TError_t try_replace_lone(Tree * program_tree, TreeNode * node,
                                 bool * success);
static TError_t try_replace_rzero(Tree * program_tree, TreeNode * node,
                                  bool * success);
static TError_t try_replace_rone(Tree * program_tree, TreeNode * node,
                                 bool * success);

const char * OUT_TREE_FILE_NAME = "meprog";


TError_t me_optimization(Tree * program_tree)
{
    MY_ASSERT(program_tree);

    TError_t tree_errors = 0;
    bool is_calculated = true;
    while (is_calculated)
    {   
        is_calculated = false;
        tree_errors |= calculation_optimization(program_tree, program_tree->root,
                                                &is_calculated);
        if (tree_errors) return tree_errors;
    }

    bool is_replaced = true;
    while (is_replaced)
    {
        is_replaced = false;
        tree_errors |= replace_optimization(program_tree, program_tree->root,
                                            &is_calculated);
        if (tree_errors) return tree_errors;
    }
    
    return tree_errors;
}


static TError_t calculation_optimization(Tree * program_tree, TreeNode * node,
                                         bool * success)
{
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(success);

    TError_t tree_errors = 0;

    if (node->left)
        tree_errors |= calculation_optimization(program_tree, node->left,
                                              success);
    if (node->right)
        tree_errors |= calculation_optimization(program_tree, node->right,
                                              success);
    if (tree_errors) return tree_errors; 
    
    return try_calculate_node(program_tree, node, success);
}


static TError_t try_calculate_node(Tree * program_tree, TreeNode * node,
                                   bool * success)
{
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(success);

    if (!node->right || !node->left ||
        node->right->value.type != TOKEN_TYPES_NUMBER ||
        node->left->value.type  != TOKEN_TYPES_NUMBER ||
        node->value.type != TOKEN_TYPES_OPERATOR)
        return 0;

    node->value.val.num = calculate_new_value(program_tree, node);
    node->value.type = TOKEN_TYPES_NUMBER;

    TError_t tree_errors  = tree_delete_branch(program_tree, &(node->left));
             tree_errors |= tree_delete_branch(program_tree, &(node->right));
    if (tree_errors) return tree_errors;

    *success = true;
    return tree_errors;
}


static double calculate_new_value(Tree * program_tree, TreeNode * node)
{
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(node->value.type == TOKEN_TYPES_OPERATOR);
    MY_ASSERT(node->left->value.type  == TOKEN_TYPES_NUMBER);
    MY_ASSERT(node->right->value.type == TOKEN_TYPES_NUMBER);

    double lnode_val = node->left->value.val.num;
    double rnode_val = node->right->value.val.num;
    switch (node->value.val.op_id)
    {
        case OPERATORS_ADDITION:    
            return lnode_val + rnode_val;
            break;
        
        case OPERATORS_SUBTRACTION:
            return lnode_val - rnode_val;
            break;

        case OPERATORS_MULTIPLICATION:
            return lnode_val * rnode_val;
            break;

        case OPERATORS_DIVISION:
            return lnode_val / rnode_val;
            break;

        case OPERATORS_POWER:
            return pow(lnode_val, rnode_val);
            break;

        case OPERATORS_BELOW:
            return lnode_val < rnode_val ? 1 : 0;
            break;

        case OPERATORS_ABOVE:
            return lnode_val > rnode_val ? 1 : 0;
            break;

        case OPERATORS_EQUAL:
            return is_equal_double(lnode_val, rnode_val) ? 1 : 0;
            break;

        case OPERATORS_ASSIGMENT:
        case OPERATORS_END_OF_OP:
        default:
            MY_ASSERT(0 && "UNREACHABLE");
    }

    return 0;
}


static TError_t replace_optimization(Tree * program_tree, TreeNode * node, 
                                     bool * success)
{
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(success);

    TError_t tree_errors = 0;

    if (node->left)
        tree_errors |= replace_optimization(program_tree, node->left,
                                              success);
    if (node->right)
        tree_errors |= replace_optimization(program_tree, node->right,
                                              success);
    if (tree_errors) return tree_errors; 
    
    return try_replace_node(program_tree, node, success);
}


static TError_t try_replace_node(Tree * program_tree, TreeNode * node,
                                 bool * success)
{
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(success);

    if (!node->right || !node->left ||
        node->value.type != TOKEN_TYPES_OPERATOR)
        return 0;
    
    TError_t tree_errors = 0;
    if (node->left->value.type == TOKEN_TYPES_NUMBER)
    {
        double lnode_val = node->left->value.val.num;

        if (is_equal_double(lnode_val, 0))
        {
            tree_errors |= try_replace_lzero(program_tree, node,
                                             success);
        }
        else if (is_equal_double(lnode_val, 1))
        {
            tree_errors |= try_replace_lone(program_tree, node,
                                            success);
        }
    }
    else if (node->right->value.type == TOKEN_TYPES_NUMBER)
    {
        double rnode_val = node->right->value.val.num;

        if (is_equal_double(rnode_val, 0))
        {
            tree_errors |= try_replace_rzero(program_tree, node,
                                             success);
        }
        else if (is_equal_double(rnode_val, 1))
        {
            tree_errors |= try_replace_rone(program_tree, node,
                                            success);
        }
    }

    return tree_errors;
}
                                    

static TError_t try_replace_lzero(Tree * program_tree, TreeNode * node,
                                  bool * success)
{
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(success);
    MY_ASSERT(node->left);
    MY_ASSERT(node->right);
    MY_ASSERT(node->left->value.type == TOKEN_TYPES_NUMBER);
    MY_ASSERT(is_equal_double(node->left->value.val.num, 0));

    switch (node->value.val.op_id)
    {
        case OPERATORS_ADDITION:
            node->value = node->right->value;
            break;
        
        case OPERATORS_MULTIPLICATION:
        case OPERATORS_DIVISION:
        case OPERATORS_POWER:
            node->value.type = TOKEN_TYPES_NUMBER;
            node->value.val.num = 0;
            break;

        case OPERATORS_SUBTRACTION:
        case OPERATORS_ASSIGMENT:
        case OPERATORS_END_OF_OP:
        case OPERATORS_BELOW:
        case OPERATORS_ABOVE:
        case OPERATORS_EQUAL:
            return 0;
            break;

        default:
            MY_ASSERT(0 && "UNREACHABLE");
            break;
    }

    *success = true;
    return tree_delete_branch(program_tree, &(node->left)) ||
           tree_delete_branch(program_tree, &(node->right));
}


static TError_t try_replace_lone(Tree * program_tree, TreeNode * node,
                                 bool * success)
{
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(success);
    MY_ASSERT(node->left);
    MY_ASSERT(node->right);
    MY_ASSERT(node->left->value.type == TOKEN_TYPES_NUMBER);
    MY_ASSERT(is_equal_double(node->left->value.val.num, 1));

    switch (node->value.val.op_id)
    {
        case OPERATORS_MULTIPLICATION:
            node->value = node->right->value;
            break;

        case OPERATORS_POWER:
            node->value.type = TOKEN_TYPES_NUMBER;
            node->value.val.num = 1;
            break;

        case OPERATORS_ADDITION:
        case OPERATORS_DIVISION:
        case OPERATORS_SUBTRACTION:
        case OPERATORS_ASSIGMENT:
        case OPERATORS_END_OF_OP:
        case OPERATORS_BELOW:
        case OPERATORS_ABOVE:
        case OPERATORS_EQUAL:
            return 0;
            break;

        default:
            MY_ASSERT(0 && "UNREACHABLE");
            break;
    }

    *success = true;
    return tree_delete_branch(program_tree, &(node->left)) ||
           tree_delete_branch(program_tree, &(node->right));
}


static TError_t try_replace_rzero(Tree * program_tree, TreeNode * node,
                                  bool * success)
{
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(success);
    MY_ASSERT(node->left);
    MY_ASSERT(node->right);
    MY_ASSERT(node->right->value.type == TOKEN_TYPES_NUMBER);
    MY_ASSERT(is_equal_double(node->right->value.val.num, 0));

    switch (node->value.val.op_id)
    {
        case OPERATORS_ADDITION:
        case OPERATORS_SUBTRACTION:
            node->value = node->left->value;
            break;
        
        case OPERATORS_MULTIPLICATION:
            node->value.type = TOKEN_TYPES_NUMBER;
            node->value.val.num = 0;
            break;

        case OPERATORS_POWER:
            node->value.type = TOKEN_TYPES_NUMBER;
            node->value.val.num = 1;
            break;

        case OPERATORS_DIVISION:
        case OPERATORS_ASSIGMENT:
        case OPERATORS_END_OF_OP:
        case OPERATORS_BELOW:
        case OPERATORS_ABOVE:
        case OPERATORS_EQUAL:
            return 0;
            break;

        default:
            MY_ASSERT(0 && "UNREACHABLE");
            break;
    }

    *success = true;
    return tree_delete_branch(program_tree, &(node->left)) ||
           tree_delete_branch(program_tree, &(node->right));
}


static TError_t try_replace_rone(Tree * program_tree, TreeNode * node,
                                 bool * success)
{
    MY_ASSERT(program_tree);
    MY_ASSERT(node);
    MY_ASSERT(success);
    MY_ASSERT(node->left);
    MY_ASSERT(node->right);
    MY_ASSERT(node->right->value.type == TOKEN_TYPES_NUMBER);
    MY_ASSERT(is_equal_double(node->right->value.val.num, 1));

    switch (node->value.val.op_id)
    {
        case OPERATORS_MULTIPLICATION:
        case OPERATORS_DIVISION:
        case OPERATORS_POWER:
            node->value = node->right->value;
            break;

        case OPERATORS_ADDITION:
        case OPERATORS_SUBTRACTION:
        case OPERATORS_ASSIGMENT:
        case OPERATORS_END_OF_OP:
        case OPERATORS_BELOW:
        case OPERATORS_ABOVE:
        case OPERATORS_EQUAL:
            return 0;
            break;

        default:
            MY_ASSERT(0 && "UNREACHABLE");
            break;
    }

    *success = true;
    return tree_delete_branch(program_tree, &(node->left)) ||
           tree_delete_branch(program_tree, &(node->right));
}