#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "vtor.h"
#include "cmd_input.h"
#include "my_assert.h"
#include "arithmetic.h"
#include "file_processing.h"
#include "spu.h"
#include "processor.h"
#include "flags.h"


int main(int argc, char * argv[])
{
    if (!check_cmd_input(argc, argv))
        return 1;

    SoftProcessorUnit spu = {};
    Error_t errors = 0;
    FILE * fp = 0;

    if ((errors = stack_ctor(&spu.stk)))
        return errors;

    if ((errors = stack_ctor(&spu.call_stk)))
        return errors;

    if (!(fp = file_open(SPU_FILE_NAME, "rb")))
        return 1;

    size_t file_size = get_file_size(fp);
    if ((spu.bytecode = (char *) calloc(file_size, sizeof(char))) == NULL)
    {
        printf("Error. Can't allocate a memmory.\n");
        fclose(fp);
        return 1;
    }
    spu.bytecode_size = file_size;

    if ((spu.bytecode = read_file(spu.bytecode, file_size, fp)) == NULL)
    {
        fclose(fp);
        free(spu.bytecode);
        return 1;
    }

    if ((errors = spu_process_comands(&spu)))
    {
        spu_dump(&spu, &errors);
        fclose(fp);
        free(spu.bytecode);
        return errors;
    }
    fclose(fp);

    free(spu.bytecode);
    if ((errors = stack_dtor(&spu.stk)))
        return errors;

    return 0;
}
