#ifndef MIDDLE_END_H
    #define MIDDLE_END_H

    #include "tree.h"

    enum MiddleEndErrorMasks {
        MIDDLE_END_ERROR_CANT_READ_TREE = 1 << 0,
    };

    extern const char * OUT_TREE_FILE_NAME;

    TError_t me_optimization(Tree * program_tree);

#endif // MIDDLE_END_H  