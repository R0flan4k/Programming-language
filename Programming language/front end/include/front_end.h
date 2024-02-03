#ifndef FRONT_END_H
    #define FRONT_END_H

    #include "operators.h"
    #include "key_words.h"
    #include "name_table.h"

    typedef int FrontEndErrors;

    enum FrontEndErrorMasks {
        FRONT_END_ERRORS_CANT_CONVERT_SOURCE_FILE      = 1 << 0,
        FRONT_END_ERRORS_CANT_SEPARATE_NAME_TABLE_ELEM = 1 << 1,
        FRONT_END_ERRORS_NAME_TABLE_OVERFLOW           = 1 << 2,
        FRONT_END_ERRORS_TOKENS_ARRAY_OVERFLOW         = 1 << 3,
        FRONT_END_ERRORS_CANT_FIND_TERMINATOR          = 1 << 4,
        FRONT_END_ERRORS_NO_FUNCTIONS                  = 1 << 5,
        FRONT_END_ERRORS_EMPTY_FUNCTION                = 1 << 6,
        FRONT_END_ERRORS_INVALID_RETURN_TYPE           = 1 << 7,
        FRONT_END_ERRORS_INVALID_FUNCTION_NAME         = 1 << 8,
    };

    enum TokenTypes {
        TOKEN_TYPES_NUMBER,
        TOKEN_TYPES_OPERATOR,
        TOKEN_TYPES_NAME_TABLE_ELEM,
        TOKEN_TYPES_KEY_WORD,
        TOKEN_TYPES_SYMBOL,
        TOKEN_TYPES_TERMINATOR,
    };

    union Token {
        double num;
        char sym;
        Operator op;
        KeyWord kwd;
        NameTableElem elem;
    };

    struct FrontEndToken {
        Token val;
        TokenTypes type;
    };

    struct FuncTree {
        Tree tree;
        const char * name;
    };

    const size_t MAX_TOKENS_NUMBER = 128;
    const size_t MAX_FUNCTIONS_NUMBER = 64;
    const char * MAIN_FUNCTION = "main";
    const char * MAIN_FUNCTION_LEN = sizeof("main") - 1;
    const FrontEndToken FUNCTION_OPENING = {.val = {.sym = '{'},
                                            .type = TOKEN_TYPES_SYMBOL};

    FrontEndErrors separate_tokens(char * buffer, FrontEndToken * tokens, size_t max_tokens_number,
                                   NameTable * name_table);
    void tokens_dump(const FrontEndToken * tokens);

#endif // FRONT_END_H
