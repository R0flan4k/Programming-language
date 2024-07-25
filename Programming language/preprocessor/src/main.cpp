#include <stdio.h>
#include <stdlib.h>

#include "cmd_input.h"
#include "flags.h"
#include "file_processing.h"
#include "my_assert.h"
#include "preproc.h"


int main(int argc, char * argv[])
{
    if (!check_cmd_input(argc, argv))
    {
        return 1;
    }

    char * buffer = NULL;
    long buffer_size = 0;
    if (!(buffer_size = text_file_to_buffer(PROGRAM_FILE_NAME, &buffer)))
        return 1;

    long output_buffer_size = (long) ((double) buffer_size * EXPAND_BUFFER_COEFFICIENT);
    char * output_buffer = (char *) calloc(output_buffer_size, sizeof(char));
    if (!output_buffer) return 1;

    preprocessing(buffer, output_buffer, buffer_size, output_buffer_size);

    FILE * fp = file_open(PREPROCESSED_PROG_FILE, "wb");
    if (!fp) return 1;
    fprintf(fp, "%s", output_buffer);

    fclose(fp);
    free(output_buffer);
    free(buffer);

    return 0;
}
