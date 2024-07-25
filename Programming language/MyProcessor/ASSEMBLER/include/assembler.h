#ifndef ASSEMBLER_H
    #define ASSEMBLER_H

    #include "hash.h"
    #include "stack.h"
    #include "processor.h"

    typedef unsigned int LabelVal_t;

    enum AssemblerErrors {
        ASSEMBLER_NO_ERRORS      = 0 << 0,
        ASSEMBLER_POP_ERROR      = 1 << 0,
        ASSEMBLER_PUSH_ERROR     = 1 << 1,
        ASSEMBLER_ARGUMENT_ERROR = 1 << 2,
        ASSEMBLER_LABEL_ERROR    = 1 << 3,
    };

    struct AssemblerLabel {
        char * name;
        Hash_t hash;
        LabelVal_t value;
    };

    struct AssemblerCommand {
        const char * command;
        int num_of_params;
        size_t size;
        const Hash_t hash;
        int signature;
        ProcessorCommands command_number;
    };

    struct AssemblerRegister {
        const char * rgstr;
        const Hash_t hash;
        ProcessorRegisters id;
    };

    typedef AssemblerLabel Label_t;

    const int NECESSARY_CONVERT_NUMBER = 2;
    const size_t LABELS_ARRAY_SIZE = 32;

    AssemblerErrors assembler_convert(char const * const * asm_strs, size_t strings_num,
                                      char * output_buffer, Label_t * labels, size_t labels_size,
                                      int convertor_call_number);
    void assembler_dump(char const * const * asm_strs, size_t strings_num, const char * output_buffer, size_t buffer_size);
    AssemblerErrors labels_dtor(Label_t * labels, size_t size);

#endif // ASSEMBLER_H
