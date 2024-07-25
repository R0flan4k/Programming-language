#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "assembler.h"
#include "hash.h"
#include "my_assert.h"
#include "strings.h"
#include "stack.h"
#include "processor.h"

static bool try_process_signature_number(const char * * buffer, char * * output_buffer);
static bool try_process_signature_register(const char * * buffer, char * * output_buffer);
static bool try_process_signature_label(const char * * buffer, char * * output_buffer,
                                        Label_t * labels, size_t size,
                                        int convertor_call_number);
static bool try_process_signature_ram(const char * * buffer, char * * output_buffer);
static bool is_label(const char * string);
static AssemblerErrors add_label(const char * string, Label_t * labels, size_t size, int ip);
static AssemblerErrors assembler_argument_processing(const char * * buffer, char * * output_buffer, int signature,
                                                     Label_t * labels, size_t size, int convertor_call_number);
static AssemblerCommand assembler_create_command(const char * command_str, int signature, ProcessorCommands id, int number_of_params);
static AssemblerRegister assembler_create_register(const char * register_str, ProcessorRegisters id);

const size_t MAX_COMMAND_SIZE = 64;

AssemblerCommand ASSEMBLER_COMMANDS_ARRAY[] = {
    #define CMD(str, signature, enum_id, num_of_params, id, code) \
        assembler_create_command(str, signature, PROCESSOR_COMMAND_ ## enum_id, num_of_params),

    #include "commands.h"

    #undef CMD
};
size_t ASSEMBLER_COMMANDS_ARRAY_SIZE = sizeof(ASSEMBLER_COMMANDS_ARRAY) /
                                       sizeof(ASSEMBLER_COMMANDS_ARRAY[0]);

AssemblerRegister ASSEMBLER_REGISTERS_ARRAY[] = {
    assembler_create_register("rax", PROCESSOR_REGISTER_RAX),
    assembler_create_register("rbx", PROCESSOR_REGISTER_RBX),
    assembler_create_register("rcx", PROCESSOR_REGISTER_RCX),
    assembler_create_register("rdx", PROCESSOR_REGISTER_RDX),
    assembler_create_register("ip",  PROCESSOR_REGISTER_IP),
};
size_t ASSEMBLER_REGISTERS_ARRAY_SIZE = sizeof(ASSEMBLER_COMMANDS_ARRAY) /
                                        sizeof(ASSEMBLER_COMMANDS_ARRAY[0]);


AssemblerErrors assembler_convert(char const * const * asm_strs, size_t strings_num,
                                  char * start_output_buffer, Label_t * labels, size_t labels_size,
                                  int convertor_call_number)
{
    MY_ASSERT(asm_strs && start_output_buffer);

    AssemblerErrors error = ASSEMBLER_NO_ERRORS;

    char * output_buffer = start_output_buffer;

    for (size_t i = 0; i < strings_num; i++)
    {
        char const * buffer_ptr = asm_strs[i];
        buffer_ptr = skip_spaces(buffer_ptr);

        if (is_label(buffer_ptr))
        {
            if (error = add_label(buffer_ptr, labels, labels_size, (int) (output_buffer - start_output_buffer)))
                return error;

            continue;
        }

        char command[MAX_COMMAND_SIZE] = "";
        int cmd_len = 0;
        sscanf(buffer_ptr, "%s%n", command, &cmd_len);
        buffer_ptr += cmd_len;
        Hash_t string_hash = calculate_hash(command, cmd_len + 1);
        for (size_t j = 0; j < ASSEMBLER_COMMANDS_ARRAY_SIZE; j++)
        {
            if (string_hash == ASSEMBLER_COMMANDS_ARRAY[j].hash)
            {
                *output_buffer = (char) ASSEMBLER_COMMANDS_ARRAY[j].command_number;
                output_buffer++;

                for (int k = 0; k < ASSEMBLER_COMMANDS_ARRAY[j].num_of_params; k++)
                {
                    if ((error = assembler_argument_processing(&buffer_ptr, &output_buffer, ASSEMBLER_COMMANDS_ARRAY[j].signature,
                         labels, labels_size, convertor_call_number)))
                    {
                        return error;
                    }

                    buffer_ptr = skip_word(buffer_ptr);
                }
            }
        }
    }

    return ASSEMBLER_NO_ERRORS;
}


static AssemblerErrors assembler_argument_processing(const char * * buffer, char * * output_buffer, int signature,
                                                     Label_t * labels, size_t size, int convertor_call_number)
{
    bool success = false;
    *buffer = skip_spaces(*buffer);

    if (signature & PROCESSOR_SIGNATURE_EMPTY)
    {
        return ASSEMBLER_NO_ERRORS;
    }

    if (signature & PROCESSOR_SIGNATURE_NUMBER)
    {
        success = try_process_signature_number(buffer, output_buffer);
    }

    if (signature & PROCESSOR_SIGNATURE_REGISTER && success == false)
    {
        success = try_process_signature_register(buffer, output_buffer);
    }

    if (signature & PROCESSOR_SIGNATURE_LABEL && success == false)
    {
        success = try_process_signature_label(buffer, output_buffer, labels, size, convertor_call_number);
    }

    if (signature & PROCESSOR_SIGNATURE_RAM && success == false)
    {
        success = try_process_signature_ram(buffer, output_buffer);
    }

    if (success == false)
    {
        return ASSEMBLER_ARGUMENT_ERROR;
    }

    return ASSEMBLER_NO_ERRORS;
}


static bool try_process_signature_number(const char * * buffer, char * * output_buffer)
{
    Elem_t val = 0;

    if (sscanf(*buffer, ELEM_SPEC, &val) == 1)
    {
        *buffer = skip_word(*buffer);
        *(*output_buffer) = (char) PROCESSOR_SIGNATURE_NUMBER;
        (*output_buffer)++;
        for (size_t k = 0; k < sizeof(Elem_t); k++)
        {
            *(*output_buffer) = *((char *) &val + k);
            (*output_buffer)++;
        }

        return true;
    }

    return false;
}


static bool try_process_signature_register(const char * * buffer, char * * output_buffer)
{
    char val_string[MAX_COMMAND_SIZE] = "";
    int val_string_len = 0;

    if (sscanf(*buffer, "%s%n", val_string, &val_string_len))
    {
        Hash_t val_string_hash = calculate_hash(val_string, val_string_len + 1);

        for (size_t i = 0; i < ASSEMBLER_REGISTERS_ARRAY_SIZE; i++)
        {
            if (val_string_hash == ASSEMBLER_REGISTERS_ARRAY[i].hash)
            {
                *buffer += val_string_len;
                *(*output_buffer) = (char) PROCESSOR_SIGNATURE_REGISTER;
                (*output_buffer)++;
                *(*output_buffer) = (char) ASSEMBLER_REGISTERS_ARRAY[i].id;
                (*output_buffer)++;
                return true;
            }
        }
    }

    return false;
}

static bool try_process_signature_label(const char * * buffer, char * * output_buffer,
                                        Label_t * labels, size_t size,
                                        int convertor_call_number)
{
    if (**buffer == ':')
    {
        (*buffer)++;
        char label_name[MAX_COMMAND_SIZE] = "";
        int label_len = 0;
        sscanf(*buffer, "%s%n", label_name, &label_len);
        *buffer += label_len;
        Hash_t label_hash = calculate_hash(label_name, label_len + 1);

        size_t label_id = 0;
        if (convertor_call_number == NECESSARY_CONVERT_NUMBER - 1)
        {
            for (label_id = 0; label_id < size; label_id++)
            {
                if (labels[label_id].hash == label_hash)
                {
                    // *(*output_buffer) = (char) labels[label_id].value;
                    // (*output_buffer)++;
                    size_t label_val = labels[label_id].value;
                    for (size_t k = 0; k < sizeof(LabelVal_t); k++)
                    {
                        *(*output_buffer) = *((char *) &label_val + k);
                        (*output_buffer)++;
                    }

                    break;
                }
            }
        }
        else
        {
            (*output_buffer) += sizeof(LabelVal_t);
        }

        return true;
    }

    return false;
}


static bool try_process_signature_ram(const char * * buffer, char * * output_buffer)
{
    *buffer = skip_spaces(*buffer);

    if (**buffer == '[')
    {
        *buffer = skip_spaces(*buffer);
        *(*output_buffer) = (char) PROCESSOR_SIGNATURE_RAM;
        (*output_buffer)++;
        (*buffer)++;

        bool success = false;

        if ((success = try_process_signature_number(buffer, output_buffer)))
        {
            return success;
        }
        else if ((success = try_process_signature_register(buffer, output_buffer)))
        {
            return success;
        }

        *buffer = skip_spaces(*buffer);

        if (**buffer == ']')
            return true;
    }

    return false;
}


static AssemblerCommand assembler_create_command(const char * command_str, int signature, ProcessorCommands id, int number_of_params)
{
    AssemblerCommand command = {
        .command = command_str,
        .num_of_params = number_of_params,
        .hash = calculate_hash(const_cast <char *> (command_str), strlen(command_str) + 1),
        .signature = signature,
        .command_number = id,
    };

    return command;
}


static AssemblerRegister assembler_create_register(const char * register_str, ProcessorRegisters id)
{
    AssemblerRegister rgstr = {
        .rgstr = register_str,
        .hash = calculate_hash(const_cast <char *> (register_str), strlen(register_str) + 1),
        .id = id,
    };

    return rgstr;
}


static bool is_label(const char * string)
{
    string = skip_spaces(string);
    return (*string == ':');
}


static AssemblerErrors add_label(const char * string, Label_t * labels, size_t size, int ip)
{
    MY_ASSERT(string);
    MY_ASSERT(labels);

    char * ptr = const_cast <char *> (strchr(string, ':') + 1);

    size_t label_id = 0;
    for (label_id = 0; label_id < size; label_id++)
    {
        if (!labels[label_id].name)
            break;
    }

    if (label_id > size)
        return ASSEMBLER_LABEL_ERROR;

    char label_name[MAX_COMMAND_SIZE] = "";
    int label_len = 0;
    sscanf(ptr, "%s%n", label_name, &label_len);

    if (!(labels[label_id].name = (char *) calloc(label_len + 1, sizeof(char))))
        return ASSEMBLER_LABEL_ERROR;

    strcpy(labels[label_id].name, label_name);
    labels[label_id].value = ip;

    labels[label_id].hash = calculate_hash(labels[label_id].name, label_len + 1);

    return ASSEMBLER_NO_ERRORS;
}


AssemblerErrors labels_dtor(Label_t * labels, size_t size)
{
    for (size_t i = 0; i < size && labels[i].name; i++)
    {
        free(labels[i].name);
    }

    return ASSEMBLER_NO_ERRORS;
}


void assembler_dump(char const * const * asm_strs, size_t strings_num, const char * output_buffer, size_t buffer_size)
{
    MY_ASSERT(asm_strs);
    MY_ASSERT(output_buffer);

    printf("========================================================\n");
    printf("--------------------ASSEMBLER-DUMP----------------------\n");
    printf("|                                                      |\n");
    printf("|Inputed text:                                         |\n");

    for (size_t i = 0; i < strings_num; i++)
    {
        printf("|");
        printf("    ");
        size_t j = 0;
        for (; asm_strs[i][j] != '\0' && j < 50; j++)
        {
            printf("%c", asm_strs[i][j]);
        }
        while (j < 50)
        {
            printf(" ");
            j++;
        }
        printf("|\n");
    }

    printf("|Outputed bytecode:                                    |\n");

    char byte[9] = "";
    for (size_t i = 0; i < buffer_size / 16; i++)
    {
        printf("|");
        printf("    ");
        for (size_t j = 0; j < 16; j++)
        {
            sprintf(byte, "%-.8X", (char) output_buffer[i * 16 + j]);
            printf("%s ", byte + 6);
        }
        printf( "  |\n");
    }

    printf("========================================================\n");
}
