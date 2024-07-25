#ifndef FLAGS_H
    #define FLAGS_H

    #include "cmd_input.h"

    extern CmdLineArg ASSEMBLER_SOURCE_FILE;
    extern CmdLineArg ASSEMBLER_TARGET_FILE;

    extern char * SOURCE_FILE_NAME;
    extern char * TARGET_FILE_NAME;

    extern char * * cmd_input;
    extern CmdLineArg * FLAGS[];
    extern size_t FLAGS_ARRAY_SIZE;

    void show_error_message(const char * program_name);
    void set_assembler_source_file_name_flag(void);
    void set_assembler_target_file_name_flag(void);

#endif // FLAGS_H
