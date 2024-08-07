#ifndef FRONT_END_H
    #define FRONT_END_H

    #include "operators.h"
    #include "key_words.h"
    #include "name_table.h"
    #include "tree.h"

    typedef int FrontEndErrors;

    enum FrontEndErrorMasks {
        FRONT_END_ERRORS_CANT_CONVERT_SOURCE_FILE      = 1 << 0,
        FRONT_END_ERRORS_CANT_SEPARATE_NAME_TABLE_ELEM = 1 << 1,
        FRONT_END_ERRORS_NAME_TABLE_OVERFLOW           = 1 << 2,
        FRONT_END_ERRORS_TOKENS_ARRAY_OVERFLOW         = 1 << 3,
        FRONT_END_ERRORS_CANT_FIND_TERMINATOR          = 1 << 4,
        FRONT_END_ERRORS_NO_FUNCTIONS                  = 1 << 5,
        FRONT_END_ERRORS_NO_OPEN_BRAKET                = 1 << 6,
        FRONT_END_ERRORS_DATA_TYPE_EXPECTED            = 1 << 7,
        FRONT_END_ERRORS_INVALID_FUNCTION_NAME         = 1 << 8,
        FRONT_END_ERRORS_TREE_ERROR                    = 1 << 9,
        FRONT_END_ERRORS_INVALID_BODY_CONSTRUCTION     = 1 << 10,
        FRONT_END_ERRORS_NO_CLOSE_BRAKET               = 1 << 11,
        FRONT_END_ERRORS_UNDEFINED_VAR                 = 1 << 12,
        FRONT_END_ERRORS_EXPRESSION_EXPECTED           = 1 << 13,
        FRONT_END_ERRORS_INVALID_ARGUMENT              = 1 << 14,
        FRONT_END_ERRORS_END_OF_OP_EXPECTED            = 1 << 15,
        FRONT_END_ERRORS_INVALID_VAR_NAME              = 1 << 16,
        FRONT_END_ERRORS_DEFINITION_EXPECTED           = 1 << 17,
        FRONT_END_ERRORS_NO_MAIN_FUNCTION              = 1 << 18,
    };

    const size_t MAX_TOKENS_NUMBER = 256;
    extern const char * TREE_FILE_NAME;

    FrontEndErrors separate_tokens(char * buffer, LangToken * tokens,
                                   size_t max_tokens_number, NameTable * name_table);
    FrontEndErrors get_program_code(LangToken * tokens, NameTable * name_table,
                                    Tree * program_tree,
                                    size_t * token_index);
    void fe_error_output(FrontEndErrors fe_errors, size_t token_inedex);
    void tokens_dump(const LangToken * tokens, NameTable * name_table);

#endif // FRONT_END_H
