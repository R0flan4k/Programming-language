#include "key_words.h"
#include "my_assert.h"

static size_t len(size_t size);
static KeyWord create_key_word(const char * kwd_name, size_t kwd_len, KeyWordIDs kwd_id);

KeyWord KEY_WORDS_ARRAY[] = {
    create_key_word("mmaattyr",      len(sizeof("mmaattyr")),      KEY_WORDS_INT),
    create_key_word("pyldaar",       len(sizeof("pyldaar")),       KEY_WORDS_DOUBLE),
    create_key_word("tteereess",     len(sizeof("tteereess")),     KEY_WORDS_BOOL),
    create_key_word("ssaasspaallii", len(sizeof("ssaasspaallii")), KEY_WORDS_CHAR),
    create_key_word("scyk",          len(sizeof("scyk")),          KEY_WORDS_VOID),
    create_key_word("aascttaa",      len(sizeof("aascttaa")),      KEY_WORDS_IF),
    create_key_word("kyndaa",        len(sizeof("kyndaa")),        KEY_WORDS_WHILE),
    create_key_word("ssihvpyl",      len(sizeof("ssihvpyl")),      KEY_WORDS_RETURN),
    create_key_word("aaijdaa",       len(sizeof("aaijdaa")),       KEY_WORDS_FUNCTION_START),
    create_key_word("uraattma",      len(sizeof("uraattma")),      KEY_WORDS_IN),
    create_key_word("kaajattpoor",   len(sizeof("kaajattpoor")),   KEY_WORDS_OUT),
    create_key_word("kaaijraammaar", len(sizeof("kaaijraammaar")), KEY_WORDS_BODY_START),
    create_key_word("kiilcheesc",    len(sizeof("kiilcheesc")),    KEY_WORDS_BODY_START),
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
