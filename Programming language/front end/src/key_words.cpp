#include "key_words.h"
#include "my_assert.h"

static size_t len(size_t size);
static KeyWord create_key_word(const char * kwd_name, size_t kwd_len, KeyWordIDs kwd_id);

KeyWord KEY_WORDS_ARRAY[] = {
    create_key_word("int",    len(sizeof("int")),    KEY_WORDS_INT),
    create_key_word("double", len(sizeof("double")), KEY_WORDS_DOUBLE),
    create_key_word("bool",   len(sizeof("bool")),   KEY_WORDS_BOOL),
    create_key_word("char",   len(sizeof("char")),   KEY_WORDS_CHAR),
    create_key_word("if",     len(sizeof("if")),     KEY_WORDS_IF),
    create_key_word("while",  len(sizeof("while")),  KEY_WORDS_WHILE),
    create_key_word("return", len(sizeof("return")), KEY_WORDS_RETURN),
    create_key_word("func",   len(sizeof("func")),   KEY_WORDS_FUNCTION_START),
    create_key_word("void",   len(sizeof("void")),   KEY_WORDS_VOID),
};
size_t KEY_WORDS_ARRAY_SIZE = sizeof(KEY_WORDS_ARRAY) / sizeof(KEY_WORDS_ARRAY[0]);

static KeyWord create_key_word(const char * kwd_name, size_t kwd_len, KeyWordIDs kwd_id)
{
    MY_ASSERT(kwd_name);

    KeyWord key_word = {
        .name = kwd_name,
        .length = kwd_len,
        .id = kwd_id,
    };

    return key_word;
}


static size_t len(size_t size)
{
    return size - 1;
}
