#ifndef TREE_H
    #define TREE_H

    #include <stdio.h>

    #include "operators.h"
    #include "key_words.h"
    #include "name_table.h"

    #if 0
        union TreeValue {
            double num;
            const char * string;
        };

        enum TreeNodeTypes {
            TREE_NODE_TYPES_NUMBER,
            TREE_NODE_TYPES_STRING,
        };

        struct TreeElem {
            TreeValue val;
            TreeNodeTypes type;
        };
    #endif

    typedef size_t NameTableElemId;

    enum TokenTypes {
        TOKEN_TYPES_NUMBER,
        TOKEN_TYPES_OPERATOR,
        TOKEN_TYPES_NAME_TABLE_ELEM,
        TOKEN_TYPES_KEY_WORD,
        TOKEN_TYPES_SYMBOL,
        TOKEN_TYPES_TERMINATOR,
    };

    union Token {
        double num;
        char sym;
        Operator op;
        KeyWord kwd;
        NameTableElemId elem_id;
    };

    struct FrontEndToken {
        Token val;
        TokenTypes type;
    };

    typedef FrontEndToken Tree_t;
    typedef int TError_t;

    #define TREE_SPEC "%p"
    #define tree_dump(tree, name_table) tree_dump_iternal((tree), #tree, __func__, \
                                                          __LINE__, __FILE__, (name_table))

    enum TreeErrorsMasks {
        TREE_ERRORS_CANT_ALLOCATE_MEMORY = 1 << 0,
        TREE_ERRORS_ALREADY_DESTRUCTED   = 1 << 1,
        TREE_ERRORS_INVALID_ROOT         = 1 << 2,
        TREE_ERRORS_INVALID_SIZE         = 1 << 3,
        TREE_ERRORS_INVALID_NODE         = 1 << 4,
        TREE_ERRORS_ALREADY_CONSTRUCTED  = 1 << 5,
    };

    enum TreeNodeBranches {
        TREE_NODE_BRANCH_LEFT,
        TREE_NODE_BRANCH_RIGHT,
    };

    struct TreeNode {
        Tree_t value;
        TreeNode * right;
        TreeNode * left;
        TreeNode * parent;
    };

    struct Tree {
        TreeNode * root;
        size_t size;
    };

    extern const char * TREE_DUMP_FILE_NAME;
    const size_t MAX_STR_SIZE = 64;
    const Tree_t TREE_NULL = {};
    TError_t op_new_tree(Tree * tree, const Tree_t root_value);
    TError_t op_delete_tree(Tree * tree);
    TError_t tree_vtor(const Tree * tree);
    TError_t tree_insert(Tree * tree, TreeNode * node, TreeNodeBranches mode, const Tree_t value);
    TError_t tree_create_node(Tree * tree, TreeNode * const parent_node, TreeNode * * node_ptr);
    TError_t tree_delete_branch(Tree * tree, TreeNode * * node);
    void tree_dump_iternal(const Tree * tree,
                           const char * tree_name, const char * func,
                           const int line, const char * file,
                           NameTable * name_table);
    void tree_text_dump(const Tree * tree);
    TError_t tree_copy_branch(Tree * dst_tree, TreeNode * dst_node, const TreeNode * src_node);
    void tree_save_to_file(Tree * tree, NameTable * name_table,
                           char * data_file_name);

#endif // TREE_H
