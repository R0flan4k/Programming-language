#include <stdio.h>

#include "front_end.h"
#include "file_processing.h"
#include "cmd_input.h"
#include "flags.h"


int main(int argc, char * argv[])
{
    if (!check_cmd_input(argc, argv))
    {
        return 1;
    }

    FrontEndErrors fe_errors = 0;
    char * buffer = NULL;

    long buffer_size = 0;
    if (!(buffer_size = text_file_to_buffer(SOURCE_FILE_NAME, &buffer)))
    {
        fe_errors |= FRONT_END_ERRORS_CANT_CONVERT_SOURCE_FILE;
        return fe_errors;
    }

    FrontEndToken tokens[MAX_TOKENS_NUMBER] = {};
    NameTable name_table = {};

    fe_errors |= separate_tokens(buffer, tokens, MAX_TOKENS_NUMBER, &name_table);

    if (fe_errors)
    {
        printf("Tokens separate error: %d.\n", fe_errors);
        return fe_errors;
    }

    tokens_dump(tokens);

    return fe_errors;
}
