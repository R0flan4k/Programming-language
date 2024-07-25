#include <stdio.h>
#include <string.h>

#include "cmd_input.h"
#include "my_assert.h"
#include "flags.h"

char * SOURCE_FILE_NAME = NULL;
char * TARGET_FILE_NAME = NULL;
char * * cmd_input = NULL;

CmdLineArg ASSEMBLER_SOURCE_FILE = {
    .name =          "--source",
    .num_of_param =  1,
    .flag_function = set_assembler_source_file_name_flag,
    .argc_number =   0,
    .help =          "--source *file name*",
    .is_mandatory = false,
};

CmdLineArg ASSEMBLER_TARGET_FILE = {
    .name =          "--target",
    .num_of_param =  1,
    .flag_function = set_assembler_target_file_name_flag,
    .argc_number =   0,
    .help =          "--target *file name*",
    .is_mandatory = false,
};

CmdLineArg * FLAGS[] = {&ASSEMBLER_TARGET_FILE, &ASSEMBLER_SOURCE_FILE};
size_t FLAGS_ARRAY_SIZE = sizeof(FLAGS) / sizeof(FLAGS[0]);


void show_error_message(const char * program_name)
{
    printf("Error. Please, use %s %s %s\n", program_name, ASSEMBLER_SOURCE_FILE.help, ASSEMBLER_TARGET_FILE.help);
}

void set_assembler_source_file_name_flag()
{
    SOURCE_FILE_NAME = cmd_input[ASSEMBLER_SOURCE_FILE.argc_number + 1];
}


void set_assembler_target_file_name_flag()
{
    TARGET_FILE_NAME = cmd_input[ASSEMBLER_TARGET_FILE.argc_number + 1];
}
