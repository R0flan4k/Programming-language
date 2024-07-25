#ifndef SPU_H
    #define SPU_H

    #include "stack.h"
    #include "file_processing.h"

    #define spu_dump(spu, verificator) spu_dump_iternal((spu), #spu, (verificator), __func__, __LINE__, __FILE__)

    typedef Elem_t Register_t;

    const size_t SPU_RAM_SIZE = 128;

    struct SoftProcessorUnit {
        Register_t rax;
        Register_t rbx;
        Register_t rcx;
        Register_t rdx;
        unsigned int ip;
        char * bytecode;
        size_t bytecode_size;
        Stack stk;
        Stack call_stk;
        Register_t ram[SPU_RAM_SIZE];
    };

    Error_t spu_process_comands(SoftProcessorUnit * spu);
    void spu_dump_iternal(const SoftProcessorUnit * spu, const char * spu_name, const Error_t * verificator, const char * func, const int line, const char * file);


#endif // SPU_H
