#include <stdio.h>
#include <stdlib.h>

#include "tree.h"
#include "back_end.h"
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
        return BACK_END_ERROR_CANT_READ_TREE;

    Tree program_tree = {};
    op_new_tree(&program_tree, TREE_NULL);

    TError_t tree_errors = buf_to_tree(&program_tree, buffer);
    if (tree_errors) return tree_errors;

    FILE * fp = file_open(ASM_CODE_FILE, "wb");
    if (!fp) return 1;

    print_asm_code(fp, &program_tree);

    fclose(fp);
    op_delete_tree(&program_tree);
    free(buffer);

    return 0;
}
