#include <stdio.h>
#include <stdlib.h>

#include "tree.h"
#include "middle_end.h"
#include "cmd_input.h"
#include "flags.h"
#include "file_processing.h"


int main(int argc, char * argv[])
{
    if (!check_cmd_input(argc, argv))
    {
        return 1;
    }

    char * buffer = NULL;
    long buffer_size = 0;
    if (!(buffer_size = text_file_to_buffer(PROGRAM_TREE_FILE_NAME, &buffer)))
        return MIDDLE_END_ERROR_CANT_READ_TREE;

    Tree program_tree = {};
    op_new_tree(&program_tree, TREE_NULL);

    TError_t tree_errors = buf_to_tree(&program_tree, buffer);
    if (tree_errors) return tree_errors;

    tree_errors |= me_optimization(&program_tree);
    if (tree_errors) return tree_errors;

    #ifndef NDEBUG
        tree_dump(&program_tree);
    #endif // NDEBUG
    tree_save_to_file(&program_tree, OUT_TREE_FILE_NAME);

    op_delete_tree(&program_tree);
    free(buffer);

    return 0;
}
