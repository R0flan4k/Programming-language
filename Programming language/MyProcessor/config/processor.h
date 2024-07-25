#ifndef PROCESSOR_H
    #define PROCESSOR_H

    enum ProcessorCommands {
    #define CMD(str, signature, enum_id, num_of_params, id, code) \
        PROCESSOR_COMMAND_ ## enum_id = id,

        #include "commands.h"

    #undef CMD
    };

    enum ProcessorRegisters {
        PROCESSOR_REGISTER_RAX = 1,
        PROCESSOR_REGISTER_RBX = 2,
        PROCESSOR_REGISTER_RCX = 3,
        PROCESSOR_REGISTER_RDX = 4,
        PROCESSOR_REGISTER_IP =  5,
    };

    enum ProcessorSignatures {
        PROCESSOR_SIGNATURE_NUMBER   = 1 << 0,
        PROCESSOR_SIGNATURE_REGISTER = 1 << 1,
        PROCESSOR_SIGNATURE_LABEL    = 1 << 2,
        PROCESSOR_SIGNATURE_RAM      = 1 << 3,
        PROCESSOR_SIGNATURE_EMPTY    = 1 << 4,
    };

#endif
