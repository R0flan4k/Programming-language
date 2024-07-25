#ifndef KEY_WORDS_H
    #define KEY_WORDS_H

    #include <stdio.h>

    enum KeyWordIDs {
        KEY_WORDS_INT,
        KEY_WORDS_DOUBLE,
        KEY_WORDS_BOOL,
        KEY_WORDS_CHAR,
        KEY_WORDS_VOID,

        KEY_WORDS_IF,
        KEY_WORDS_WHILE,
        KEY_WORDS_RETURN,
        KEY_WORDS_FUNCTION_START,
        KEY_WORDS_IN,
        KEY_WORDS_OUT,

        KEY_WORDS_BODY_START,
        KEY_WORDS_BODY_END,
    };

    struct KeyWord {
        const char * name;
        size_t length;
        KeyWordIDs id;
    };

    extern KeyWord KEY_WORDS_ARRAY[];
    extern size_t KEY_WORDS_ARRAY_SIZE;

#endif // KEY_WORDS_H
