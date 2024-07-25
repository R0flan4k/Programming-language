#include <stdio.h>
#include <math.h>

#include "arithmetic.h"
#include "stack.h"
#include "my_assert.h"
#include "spu.h"
#include "processor.h"
#include "vtor.h"
#include "double_comparing.h"

struct StackError {
    StackErrorsMasks mask;
    const char * output_error;
};

const size_t BYTE_SIZE = 8;
const size_t SPU_DUMP_BYTECODE_WIDTH = 16;

static StackError create_stack_error(StackErrorsMasks error_mask, const char * error_output_info);
static void spu_stack_dump(const Stack * stk, const char * stk_name, const Error_t * verificator);
// static Error_t argument_processing(SoftProcessorUnit * spu, char * bytecode_ptr,
                                //    void * value1, void * value2,
                                //    void (*operation)(int, void *, void *));
// static void assign_operation(int signature, void * val, void * val2);



Error_t get_two_values(Stack * stk, Elem_t * val1, Elem_t * val2)
{
    Error_t errors = 0;

    if ((errors = stack_pop(stk, val1)))
        return errors;

    if ((errors = stack_pop(stk, val2)))
        return errors;

    return errors;
}


Error_t spu_binary(Stack * stk, SpuBinaryMathOperations mode)
{
    Error_t errors = 0;
    Elem_t val1 = 0, val2 = 0, result = 0;

    if ((errors = get_two_values(stk, &val1, &val2)))
        return errors;

    switch (mode)
    {
        case BINARY_MATH_OPERATION_ADDITION:
            result = val2 + val1;
            break;

        case BINARY_MATH_OPERATION_SUBTRACTION:
            result = val2 - val1;
            break;

        case BINARY_MATH_OPERATION_MULTIPLICATION:
            result = val2 * val1;
            break;

        case BINARY_MATH_OPERATION_DIVISION:
            result = val2 / val1;
            break;

        default:
            MY_ASSERT(0 && "UNREACHABLE");
            break;
    }

    errors = stack_push(stk, result);

    return errors;
}


Error_t spu_unary(Stack * stk, SpuUnaryMathOperations mode)
{
    Error_t errors = 0;
    Elem_t val = 0, result = 0;

    if ((errors = stack_pop(stk, &val)))
        return errors;

    switch (mode)
    {
        case UNARY_MATH_OPERATION_SQUARE_ROOT:
            result = (Elem_t) sqrt((double) val);
            break;

        case UNARY_MATH_OPERATION_SINUS:
            result = (Elem_t) sin((double) val);
            break;

        case UNARY_MATH_OPERATION_COSINUS:
            result = (Elem_t) cos((double) val);
            break;

        default:
            MY_ASSERT(0 && "UNREACHABLE");
            break;
    }

    return stack_push(stk, result);
}


Error_t spu_process_comands(SoftProcessorUnit * spu)
{
    Error_t errors = 0;
    Elem_t in_val = 0;
    Elem_t out_val = 0;
    char * bytecode_ptr = spu->bytecode;

    for (spu->ip = 0; spu->ip < spu->bytecode_size && bytecode_ptr[spu->ip] != PROCESSOR_COMMAND_HLT; spu->ip++)
    {
        switch ((ProcessorCommands) bytecode_ptr[spu->ip])
        {
            #define SPU_STACK_PUSH(val) \
                if ((errors = stack_push(&spu->stk, (val))))   \
                {                                              \
                    return errors;                             \
                }

            #define SPU_STACK_POP(val) \
                if ((errors = stack_pop(&spu->stk, (val))))    \
                {                                              \
                    return errors;                             \
                }

            #define SPU_CALL_STACK_PUSH(val) \
                if ((errors = stack_push(&spu->call_stk, (val))))   \
                {                                                   \
                    return errors;                                  \
                }

            #define SPU_CALL_STACK_POP(val) \
                if ((errors = stack_pop(&spu->call_stk, val)))    \
                {                                                   \
                    return errors;                                  \
                }

            #define JUMP \
                spu->ip = (size_t) *((unsigned char *) &(bytecode_ptr[spu->ip])) - 1;

            #define JUMP_IF(exp) \
                if (exp)         \
                {                \
                    JUMP;        \
                }

            #define RETURN \
                SPU_CALL_STACK_POP(&out_val); \
                spu->ip = (size_t) out_val;   \
                spu->ip++;

            #define UNREACHABLE \
                MY_ASSERT(0 && "UNREACHABLE");

            #define CURRENT_NUMBER \
                *((Elem_t *) (bytecode_ptr + spu->ip))

            #define CURRENT_REGISTER(rgstr) \
                spu->rgstr

            #define REGISTER_PROCESS(pre_code, past_code)   \
                switch ((ProcessorRegisters) bytecode_ptr[spu->ip]) \
                {                                                   \
                    case PROCESSOR_REGISTER_RAX:                    \
                        pre_code CURRENT_REGISTER(rax) past_code;                                 \
                        break;                                      \
                                                                    \
                    case PROCESSOR_REGISTER_RBX:                    \
                        pre_code CURRENT_REGISTER(rbx) past_code;                                 \
                        break;                                      \
                                                                    \
                    case PROCESSOR_REGISTER_RCX:                    \
                        pre_code CURRENT_REGISTER(rcx) past_code;                                 \
                        break;                                      \
                                                                    \
                    case PROCESSOR_REGISTER_RDX:                    \
                        pre_code CURRENT_REGISTER(rdx) past_code;                 \
                        break;                                      \
                                                                    \
                    case PROCESSOR_REGISTER_IP:                                     \
                    default:                                        \
                        UNREACHABLE;                                \
                        break;                                      \
                }

            #define ARGUMENT_PROCESS(pre_code, past_code) \
                switch ((ProcessorSignatures) bytecode_ptr[spu->ip])         \
                {                                                            \
                    case PROCESSOR_SIGNATURE_NUMBER:                         \
                        IP++;                                      \
                        pre_code CURRENT_NUMBER past_code;                  \
                        IP += 7;                                            \
                        break;                                               \
                                                                             \
                    case PROCESSOR_SIGNATURE_REGISTER:                       \
                        IP++;                                            \
                        REGISTER_PROCESS(pre_code, past_code);              \
                        break;                                               \
                                                                             \
                    case PROCESSOR_SIGNATURE_LABEL:                          \
                        IP++;                                           \
                        pre_code *((Elem_t *) (bytecode_ptr + IP)) past_code;           \
                        break;                                               \
                                                                             \
                    case PROCESSOR_SIGNATURE_RAM:                            \
                        IP++;                                           \
                            switch ((ProcessorSignatures) bytecode_ptr[spu->ip])         \
                            {                                                            \
                            case PROCESSOR_SIGNATURE_NUMBER:                         \
                                IP++;                                      \
                                pre_code spu->ram[(int) CURRENT_NUMBER] past_code;  \
                                IP += 7;                                            \
                                break;                                               \
                                                                                    \
                            case PROCESSOR_SIGNATURE_REGISTER:                       \
                                IP++;                                            \
                                REGISTER_PROCESS(pre_code spu->ram[(int), ] past_code);              \
                                break;                                               \
                                                                                    \
                            case PROCESSOR_SIGNATURE_LABEL:                          \
                            case PROCESSOR_SIGNATURE_RAM:                            \
                            case PROCESSOR_SIGNATURE_EMPTY:                          \
                            default:                                                 \
                                UNREACHABLE                                          \
                                break;                                               \
                            }                                                       \
                        break;                                               \
                                                                             \
                    case PROCESSOR_SIGNATURE_EMPTY:                          \
                    default:                                                 \
                        UNREACHABLE                                          \
                        break;                                               \
                }

            #define SCAN_VALUE(val) \
                if (!scanf(ELEM_SPEC, val))             \
                {                                       \
                    printf("Error: Invalid input.\n");  \
                    break;                              \
                }

            #define ASSIGN_TO_REGISTER(rgstr, val) \
                CURRENT_REGISTER(rgstr) = (val);

            #define ASSIGN_TO_RAM(ram_index, val) \
                spu->ram[(int) ram_index] = (val);

            #define CANT_POP_IP_ERROR \
                printf("Error: can't pop ip register\n"); \
                return errors;

            #define IP \
                (spu->ip)

            #define SET_IP(val) \
                IP = (unsigned int) (val);

            #define THIS_BYTE \
                *((unsigned char *) &(bytecode_ptr[spu->ip]))

            #define THIS_ADDRES \
                *((unsigned int *) &(bytecode_ptr[spu->ip]))

            #define CMD(str, signature, enum_id, num_of_params, id, code)\
                case PROCESSOR_COMMAND_ ## enum_id: \
                code     \
                break;

            #include "commands.h"

            #undef CMD
            #undef IP
            #undef SET_IP
            #undef THIS_BYTE
            #undef SPU_STACK_PUSH
            #undef SPU_STACK_POP
            #undef SPU_CALL_STACK_PUSH
            #undef SPU_CALL_STACK_POP
            #undef JUMP
            #undef JUMP_IF
            #undef RETURN
            #undef UNREACHABLE
            #undef ARGUMENT_PROCESS
            #undef REGISTER_PROCESS
            #undef CURRENT_NUMBER
            #undef CURRENT_REGISTER
            #undef SCAN_VALUE
            #undef ASSIGN_TO_REGISTER
            #undef ASSIGN_TO_RAM
            #undef CANT_POP_IP_ERROR

            default:
                spu_dump(spu, &errors);
                printf("The program is empty.\n");
                return errors;
                break;
        }
    }

    return errors;
}


void spu_dump_iternal(const SoftProcessorUnit * spu, const char * spu_name, const Error_t * verificator,
                    const char * func, const int line, const char * file)
{
    MY_ASSERT(spu);
    MY_ASSERT(spu_name);
    MY_ASSERT(verificator);

    printf("===========================================================\n");
    printf("-------------------------SPU-DUMP--------------------------\n");
    printf("SoftProcessorUnit[%p] \"%s\" from %s(%d), %s\n", spu, spu_name, file, line, func);
    printf("\n");
    printf("    Registers:\n");
    printf("        rax = " ELEM_SPEC "\n", spu->rax);
    printf("        rbx = " ELEM_SPEC "\n", spu->rbx);
    printf("        rcx = " ELEM_SPEC "\n", spu->rcx);
    printf("        rdx = " ELEM_SPEC "\n", spu->rdx);
    printf("        ip  = %u\n", spu->ip);
    printf("\n");
    printf("    Bytecode size: %zd\n", spu->bytecode_size);
    printf("    Bytecode[%p]:\n", spu->bytecode);

    if (spu->ip <= spu->bytecode_size && spu->bytecode)
    {
        char byte[BYTE_SIZE + 1] = "";
        size_t strings_count = 0;
        for (; strings_count < spu->ip; strings_count += SPU_DUMP_BYTECODE_WIDTH)
        {
            printf("        ");
            for (size_t j = 0; j < SPU_DUMP_BYTECODE_WIDTH && strings_count + j < spu->bytecode_size; j++)
            {
                sprintf(byte, "%-.8X", spu->bytecode[strings_count + j]);
                printf("%s ", byte + 6);
            }
            printf("\n");
        }

        for (size_t i = 0; i < spu->ip % SPU_DUMP_BYTECODE_WIDTH; i++) printf("   ");
        printf("        ^\n");
        for (size_t i = 0; i < spu->ip % SPU_DUMP_BYTECODE_WIDTH; i++) printf("   ");
        printf("         \\\n");
        for (size_t i = 0; i < spu->ip % SPU_DUMP_BYTECODE_WIDTH; i++) printf("   ");
        printf("          ---ip\n");

        for (; strings_count < spu->bytecode_size; strings_count += SPU_DUMP_BYTECODE_WIDTH)
        {
            printf("        ");
            for (size_t j = 0; j < SPU_DUMP_BYTECODE_WIDTH && strings_count + j < spu->bytecode_size; j++)
            {
                sprintf(byte, "%-.8X", spu->bytecode[strings_count + j]);
                printf("%s ", byte + 6);
            }
            printf("\n");
        }
    }
    printf("\n");

    spu_stack_dump(&spu->stk, "Stack", verificator);
    printf("\n");
    spu_stack_dump(&spu->call_stk, "Call stack", verificator);
    printf("\n");

    printf("-----------------------------------------------------------\n");
    printf("===========================================================\n");
}


static StackError create_stack_error(StackErrorsMasks error_mask, const char * error_output_info)
{
    StackError error = {
        .mask = error_mask,
        .output_error = error_output_info,
    };

    return error;
}


static void spu_stack_dump(const Stack * stk, const char * stk_name, const Error_t * verificator)
{
    MY_ASSERT(stk);
    MY_ASSERT(stk_name);
    MY_ASSERT(verificator);

    printf("    %s[%p]:\n", stk_name, stk);
    printf("    {\n");

    printf("        hash =              %lld%s\n",
                stk->hash,     stk->hash == STACK_POISON    ? "(POISON)" : "");
    printf("        data hash =         %lld%s\n",
                stk->data_hash, stk->data_hash == STACK_POISON ? "(POISON)" : "");
    printf("        left canary =       %s%s\n",
                stk->left_jagajaga == STACK_JAGAJAGA_VALUE  ? "verified" : "spoiled",
                stk->left_jagajaga == STACK_POISON ? "(POISON)" : "");
    printf("        right canary =      %s%s\n",
                stk->right_jagajaga == STACK_JAGAJAGA_VALUE ? "verified" : "spoiled",
                stk->left_jagajaga == STACK_POISON ? "(POISON)" : "");

    if (stk->data != nullptr)
    {
    printf("        left data canary =  %s%s\n",
                *stack_get_data_left_jagajaga(stk) == STACK_JAGAJAGA_VALUE ? "verified" : "spoiled",
                *stack_get_data_left_jagajaga(stk) == STACK_POISON ? "(POISON)" : "");
    printf("        right data canary = %s%s\n",
                *stack_get_data_right_jagajaga(stk) == STACK_JAGAJAGA_VALUE ? "verified" : "spoiled",
                *stack_get_data_right_jagajaga(stk) == STACK_POISON ? "(POISON)" : "");
    }

    printf("        size =     %d%s\n",   stk->size,     stk->size == STACK_POISON     ? "(POISON)" : "");
    printf("        capacity = %d%s\n",   stk->capacity, stk->capacity == STACK_POISON ? "(POISON)" : "");
    printf("        data[%p%s]\n",        stk->data,     stk->data == STACK_POISON_PTR ? "(POISON)" : "");
    printf("        {\n");

    int i = 0;

    while (i < stk->size + 1 && stk->data != nullptr)
    {
        printf("            [%d] = " ELEM_SPEC "\n", i, stk->data[i]);

        i++;
    }

    printf("        }\n");
    printf("    }\n");

    if (*verificator)
    {
        static StackError const errors[] = {
            create_stack_error(STACKERRORS_INVALID_SIZE,
                               "Invalid size.\n"),
            create_stack_error(STACKERRORS_INVALID_CAPACITY,
                               "Invalid capacity.\n"),
            create_stack_error(STACKERRORS_INVALID_SIZECAPACITY,
                               "Capacity less than size.\n"),
            create_stack_error(STACKERRORS_INVALID_DATA,
                               "Data is nullptr.\n"),
            create_stack_error(STACKERRORS_CANT_ALLOCATE_MEMORY,
                               "Can't allocate a memory.\n"),
            create_stack_error(STACKERRORS_CANT_CONSTRUCT,
                               "Can't construct a constructed stack.\n"),
            create_stack_error(STACKERRORS_CANT_DESTRUCT,
                               "Can't destruct a destructed stack.\n"),
            create_stack_error(STACKERRORS_CANT_CONSTRICT,
                               "Can't constrict the stack capacity.\n"),
            create_stack_error(STACKERRORS_EMPTY_STACK,
                               "Can't pop an empty stack.\n"),
            create_stack_error(STACKERRORS_SPOILED_LEFT_JAGAJAGA,
                               "Spoiled left canary.\n"),
            create_stack_error(STACKERRORS_SPOILED_RIGHT_JAGAJAGA,
                               "Spoiled right canary.\n"),
            create_stack_error(STACKERRORS_SPOILED_DATA_LEFT_JAGAJAGA,
                               "Spoiled left data canary.\n"),
            create_stack_error(STACKERRORS_SPOILED_DATA_RIGHT_JAGAJAGA,
                               "Spoiled right data canary.\n"),
            create_stack_error(STACKERRORS_SPOILED_HASH_VALUE,
                               "Spoiled hash value.\n"),
            create_stack_error(STACKERRORS_SPOILED_DATA_HASH_VALUE,
                               "Spoiled data hash value.\n"),
        };

        printf("    Errors:\n");

        size_t array_size = sizeof(errors) / sizeof(errors[0]);
        size_t j = 0;

        while (j < array_size)
        {
            if (errors[j].mask & *verificator)
                printf(RED_COLOR "    %s" DEFAULT_COLOR, errors[j].output_error);

            j++;
        }
    }
}


// static Error_t argument_processing(SoftProcessorUnit * spu, char * bytecode_ptr,
//                                    void * value1, void * value2,
//                                    void (*operation)(int, void *, void *))
// {
//     Error_t errors = 0;
//
//     switch ((ProcessorSignatures) bytecode_ptr[spu->ip])
//     {
//         case PROCESSOR_SIGNATURE_NUMBER:
//             spu->ip++;
//             operation(PROCESSOR_SIGNATURE_NUMBER,
//                       value1, value2);
//             spu->ip += 7;
//             break;
//
//         case PROCESSOR_SIGNATURE_REGISTER:
//             spu->ip++;
//             operation(PROCESSOR_SIGNATURE_REGISTER,
//                       value1, value2);
//             break;
//
//         case PROCESSOR_SIGNATURE_RAM:
//             spu->ip++;
//             argument_processing(spu, bytecode_ptr,
//                                 value1, value2,
//                                 operation);
//             break;
//
//         case PROCESSOR_SIGNATURE_LABEL:
//             spu->ip++;
//             operation(PROCESSOR_SIGNATURE_LABEL,
//                       value1, value2);
//             break;
//
//         case PROCESSOR_SIGNATURE_EMPTY:
//             spu->ip++;
//             operation(PROCESSOR_SIGNATURE_EMPTY,
//                       value1, value2);
//             break;
//
//         default:
//             MY_ASSERT(0 && "UNREACHABLE");
//             break;
//     }
//
//     return errors;
// }
//
//
// static void assign_operation(int signature, void * val1, void * val2)
// {
//     if (signature & PROCESSOR_SIGNATURE_NUMBER)
//     {
//         Elem_t * value1 = (Elem_t *) val1;
//         Elem_t * value2 = (Elem_t *) val2;
//
//         *value1 = *value2;
//         printf("Value1: %lf\n", *value1);
//         printf("Value2: %lf\n", *value2);
//     }
//     else if (signature & PROCESSOR_SIGNATURE_REGISTER ||
//              signature & PROCESSOR_SIGNATURE_RAM)
//     {
//         Register_t * value1 = (Register_t *) val1;
//         Register_t * value2 = (Register_t *) val2;
//
//         *value1  = *value2;
//     }
//     else
//     {
//         ;
//     }
//
//     return;
// }
