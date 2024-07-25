#ifndef FLAGS_H
    #define FLAGS_H

    #include "cmd_input.h"

    extern CmdLineArg PROGRAM_TREE_FILE;

    extern char * PROGRAM_TREE_FILE_NAME;

    extern char * * cmd_input;
    extern CmdLineArg * FLAGS[];
    extern size_t FLAGS_ARRAY_SIZE;

    void show_error_message(const char * program_name);
    void set_program_tree_file_name_flag(void);

#endif // FLAGS_H
