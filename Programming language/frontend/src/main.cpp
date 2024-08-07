#include <stdio.h>
#include <stdlib.h>

#include "front_end.h"
#include "file_processing.h"
#include "cmd_input.h"
#include "flags.h"


int main(int argc, char * argv[])
{
    if (!check_cmd_input(argc, argv))
    {
        return 1;
    }

    FrontEndErrors fe_errors = 0;
    char * buffer = NULL;

    long buffer_size = 0;
    if (!(buffer_size = text_file_to_buffer(SOURCE_FILE_NAME, &buffer)))
    {
        return FRONT_END_ERRORS_CANT_CONVERT_SOURCE_FILE;
    }

    LangToken tokens[MAX_TOKENS_NUMBER] = {};
    NameTable name_table = {};

    fe_errors |= separate_tokens(buffer, tokens, MAX_TOKENS_NUMBER, &name_table);

    if (fe_errors)
    {
        fe_error_output(fe_errors, 0);
        return fe_errors;
    }

    size_t token_index = 0;
    Tree program_tree = {};
    TError_t tree_errors = op_new_tree(&program_tree, TREE_NULL);
    if (tree_errors) return tree_errors;
    fe_errors |= get_program_code(tokens, &name_table, &program_tree, &token_index);
    if (fe_errors)
    {
        tokens_dump(tokens, &name_table);
        fe_error_output(fe_errors, token_index);
    }
    tree_save_to_file(&program_tree, TREE_FILE_NAME);
    
    #ifndef NDEBUG
        tree_dump(&program_tree);
    #endif // NDEBUG

    op_delete_tree(&program_tree);
    free(buffer);

    return fe_errors;
}
