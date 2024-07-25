#ifndef BACK_END_H
    #define BACK_END_H

    #include "tree.h"
    
    enum BackEndErrorMasks {
        BACK_END_ERROR_CANT_READ_TREE = 1 << 0,
    };

    enum ArgRegisters {
        ARG_REGISTER_RAX,
        ARG_REGISTER_RBX,
        ARG_REGISTER_RCX,
        ARG_REGISTER_RDX,
    };

    extern const char * ASM_CODE_FILE;

    void print_asm_code(FILE * fp, Tree * prog_tree);

#endif // BACK_END_H