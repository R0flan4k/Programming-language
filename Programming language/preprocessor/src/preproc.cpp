#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "preproc.h"
#include "my_assert.h"


const char * PREPROCESSED_PROG_FILE = "preprog";


static bool isalpha_rus(char sym);
static void print_rus_to_eng(char * src_buf, char * trg_buf,
                             long i, long * extra);


void preprocessing(char * src_buf, char * trg_buf,
                   long src_buf_size, long trg_buf_size)
{
    MY_ASSERT(src_buf);
    MY_ASSERT(trg_buf);

    bool pre_is_var_part = false;
    bool cur_isalpha = false, cur_is_var_part = false;
    long i = 0, extra = 0;
    for (i = 0; i < src_buf_size; i++)
    {
        cur_isalpha = isalpha_rus(src_buf[i]);

        cur_is_var_part = cur_isalpha || (src_buf[i] == '_');
        if (pre_is_var_part) cur_is_var_part |= isdigit(src_buf[i]);

        if (pre_is_var_part && !cur_is_var_part && 
            !isspace(src_buf[i]))
        {
            trg_buf[i + extra] = ' ';
            extra++;
            trg_buf[i + extra] = src_buf[i];
        }
        else if (cur_isalpha)
        {
            print_rus_to_eng(src_buf, trg_buf, 
                             i, &extra);
            i++;
        }
        else
            trg_buf[i + extra] = src_buf[i];
    
        pre_is_var_part = cur_is_var_part;
        MY_ASSERT(i + extra < trg_buf_size);
    }
    trg_buf[i + extra] = '\0';

    return;
}


static bool isalpha_rus(char sym)
{
    return sym == -48 || sym == -47;
}


static void print_rus_to_eng(char * src_buf, char * trg_buf,
                             long i, long * extra)
{
    MY_ASSERT(src_buf);
    MY_ASSERT(trg_buf);
    MY_ASSERT(extra);

    if ((RusLetterSigns) src_buf[i] == RUS_LETTER_SIGNS_FIRST)
    {
        switch ((RusLetters1st) src_buf[i + 1])
        {
            case RUS_LETTERS_LOW_AA:
                trg_buf[i + *extra]     = 'a';
                trg_buf[i + *extra + 1] = 'a';
                break;

            case RUS_LETTERS_LOW_B:
                trg_buf[i + *extra]     = 'b';
                (*extra)--;
                break;

            case RUS_LETTERS_LOW_V:
                trg_buf[i + *extra]     = 'v';
                (*extra)--;
                break;

            case RUS_LETTERS_LOW_G:
                trg_buf[i + *extra]     = 'g';
                (*extra)--;
                break;

            case RUS_LETTERS_LOW_D:
                trg_buf[i + *extra]     = 'd';
                (*extra)--;
                break;

            case RUS_LETTERS_LOW_EE:
                trg_buf[i + *extra]     = 'e';
                trg_buf[i + *extra + 1] = 'e';
                break;

            case RUS_LETTERS_LOW_JJ:
                trg_buf[i + *extra]     = 'j';
                trg_buf[i + *extra + 1] = 'j';
                break;

            case RUS_LETTERS_LOW_Z:
                trg_buf[i + *extra]     = 'z';
                (*extra)--;
                break;

            case RUS_LETTERS_LOW_II:
                trg_buf[i + *extra]     = 'i';
                trg_buf[i + *extra + 1] = 'i';
                break;

            case RUS_LETTERS_LOW_IJ:
                trg_buf[i + *extra]     = 'i';
                trg_buf[i + *extra + 1] = 'j';
                break;

            case RUS_LETTERS_LOW_K:
                trg_buf[i + *extra]     = 'k';
                (*extra)--;
                break;

            case RUS_LETTERS_LOW_L:
                trg_buf[i + *extra]     = 'l';
                (*extra)--;
                break;

            case RUS_LETTERS_LOW_MM:
                trg_buf[i + *extra]     = 'm';
                trg_buf[i + *extra + 1] = 'm';
                break;

            case RUS_LETTERS_LOW_N:
                trg_buf[i + *extra]     = 'n';
                (*extra)--;
                break;

            case RUS_LETTERS_LOW_OO:
                trg_buf[i + *extra]     = 'o';
                trg_buf[i + *extra + 1] = 'o';
                break;

            case RUS_LETTERS_LOW_P:
                trg_buf[i + *extra]     = 'p';
                (*extra)--;
                break;

            case RUS_LETTERS_UPP_AA:
                trg_buf[i + *extra]     = 'A';
                trg_buf[i + *extra + 1] = 'A';
                break;

            case RUS_LETTERS_UPP_B:
                trg_buf[i + *extra]     = 'B';
                (*extra)--;
                break;

            case RUS_LETTERS_UPP_V:
                trg_buf[i + *extra]     = 'V';
                (*extra)--;
                break;

            case RUS_LETTERS_UPP_G:
                trg_buf[i + *extra]     = 'G';
                (*extra)--;
                break;

            case RUS_LETTERS_UPP_D:
                trg_buf[i + *extra]     = 'D';
                (*extra)--;
                break;

            case RUS_LETTERS_UPP_EE:
                trg_buf[i + *extra]     = 'E';
                trg_buf[i + *extra + 1] = 'E';
                break;

            case RUS_LETTERS_UPP_JO:
                trg_buf[i + *extra]     = 'J';
                trg_buf[i + *extra + 1] = 'O';
                break;

            case RUS_LETTERS_UPP_JJ:
                trg_buf[i + *extra]     = 'J';
                trg_buf[i + *extra + 1] = 'J';
                break;

            case RUS_LETTERS_UPP_Z:
                trg_buf[i + *extra]     = 'Z';
                (*extra)--;
                break;

            case RUS_LETTERS_UPP_II:
                trg_buf[i + *extra]     = 'I';
                trg_buf[i + *extra + 1] = 'I';
                break;

            case RUS_LETTERS_UPP_IJ:
                trg_buf[i + *extra]     = 'I';
                trg_buf[i + *extra + 1] = 'J';
                break;

            case RUS_LETTERS_UPP_K:
                trg_buf[i + *extra]     = 'K';
                (*extra)--;
                break;

            case RUS_LETTERS_UPP_L:
                trg_buf[i + *extra]     = 'L';
                (*extra)--;
                break;

            case RUS_LETTERS_UPP_MM:
                trg_buf[i + *extra]     = 'M';
                trg_buf[i + *extra + 1] = 'M';
                break;

            case RUS_LETTERS_UPP_N:
                trg_buf[i + *extra]     = 'N';
                (*extra)--;
                break;

            case RUS_LETTERS_UPP_OO:
                trg_buf[i + *extra]     = 'O';
                trg_buf[i + *extra + 1] = 'O';
                break;

            case RUS_LETTERS_UPP_P:
                trg_buf[i + *extra]     = 'P';
                (*extra)--;
                break;

            case RUS_LETTERS_UPP_R:
                trg_buf[i + *extra]     = 'R';
                (*extra)--;
                break;

            case RUS_LETTERS_UPP_SS:
                trg_buf[i + *extra]     = 'S';
                trg_buf[i + *extra + 1] = 'S';
                break;

            case RUS_LETTERS_UPP_TT:
                trg_buf[i + *extra]     = 'T';
                trg_buf[i + *extra + 1] = 'T';
                break;

            case RUS_LETTERS_UPP_Y:
                trg_buf[i + *extra]     = 'Y';
                (*extra)--;
                break;

            case RUS_LETTERS_UPP_F:
                trg_buf[i + *extra]     = 'F';
                (*extra)--;
                break;

            case RUS_LETTERS_UPP_HH:
                trg_buf[i + *extra]     = 'H';
                trg_buf[i + *extra + 1] = 'H';
                break;

            case RUS_LETTERS_UPP_TS:
                trg_buf[i + *extra]     = 'T';
                trg_buf[i + *extra + 1] = 'S';
                break;

            case RUS_LETTERS_UPP_CH:
                trg_buf[i + *extra]     = 'C';
                trg_buf[i + *extra + 1] = 'H';
                break;

            case RUS_LETTERS_UPP_SH:
                trg_buf[i + *extra]     = 'S';
                trg_buf[i + *extra + 1] = 'H';
                break;

            case RUS_LETTERS_UPP_SC:
                trg_buf[i + *extra]     = 'S';
                trg_buf[i + *extra + 1] = 'C';
                break;

            case RUS_LETTERS_UPP_TW:
                trg_buf[i + *extra]     = 'T';
                trg_buf[i + *extra + 1] = 'W';
                break;

            case RUS_LETTERS_UPP_IH:
                trg_buf[i + *extra]     = 'I';
                trg_buf[i + *extra + 1] = 'H';
                break;

            case RUS_LETTERS_UPP_MA:
                trg_buf[i + *extra]     = 'M';
                trg_buf[i + *extra + 1] = 'A';
                break;

            case RUS_LETTERS_UPP_JE:
                trg_buf[i + *extra]     = 'J';
                trg_buf[i + *extra + 1] = 'E';
                break;

            case RUS_LETTERS_UPP_U:
                trg_buf[i + *extra]     = 'U';
                (*extra)--;
                break;

            case RUS_LETTERS_UPP_JA:
                trg_buf[i + *extra]     = 'J';
                trg_buf[i + *extra + 1] = 'A';
                break;

            default:
                MY_ASSERT(0 && "UNREACHABLE");
                break;
        }
    }
    else if ((RusLetterSigns) src_buf[i] == RUS_LETTER_SIGNS_SECOND)
    {
        switch ((RusLetters2nd) src_buf[i + 1])
        {
            case RUS_LETTERS_LOW_JO:
                trg_buf[i + *extra]     = 'j';
                trg_buf[i + *extra + 1] = 'o';
                break;

            case RUS_LETTERS_LOW_R:
                trg_buf[i + *extra]     = 'r';
                (*extra)--;
                break;

            case RUS_LETTERS_LOW_SS:
                trg_buf[i + *extra]     = 's';
                trg_buf[i + *extra + 1] = 's';
                break;

            case RUS_LETTERS_LOW_TT:
                trg_buf[i + *extra]     = 't';
                trg_buf[i + *extra + 1] = 't';
                break;

            case RUS_LETTERS_LOW_Y:
                trg_buf[i + *extra]     = 'y';
                (*extra)--;
                break;

            case RUS_LETTERS_LOW_F:
                trg_buf[i + *extra]     = 'f';
                (*extra)--;
                break;

            case RUS_LETTERS_LOW_HH:
                trg_buf[i + *extra]     = 'h';
                trg_buf[i + *extra + 1] = 'h';
                break;

            case RUS_LETTERS_LOW_TS:
                trg_buf[i + *extra]     = 't';
                trg_buf[i + *extra + 1] = 's';
                break;

            case RUS_LETTERS_LOW_CH:
                trg_buf[i + *extra]     = 'c';
                trg_buf[i + *extra + 1] = 'h';
                break;

            case RUS_LETTERS_LOW_SH:
                trg_buf[i + *extra]     = 's';
                trg_buf[i + *extra + 1] = 'h';
                break;

            case RUS_LETTERS_LOW_SC:
                trg_buf[i + *extra]     = 's';
                trg_buf[i + *extra + 1] = 'c';
                break;

            case RUS_LETTERS_LOW_TW:
                trg_buf[i + *extra]     = 't';
                trg_buf[i + *extra + 1] = 'w';
                break;

            case RUS_LETTERS_LOW_IH:
                trg_buf[i + *extra]     = 'i';
                trg_buf[i + *extra + 1] = 'h';
                break;

            case RUS_LETTERS_LOW_MA:
                trg_buf[i + *extra]     = 'm';
                trg_buf[i + *extra + 1] = 'a';
                break;
            
            case RUS_LETTERS_LOW_JE:
                trg_buf[i + *extra]     = 'j';
                trg_buf[i + *extra + 1] = 'e';
                break;

            case RUS_LETTERS_LOW_U:
                trg_buf[i + *extra]     = 'u';
                (*extra)--;
                break;

            case RUS_LETTERS_LOW_JA:
                trg_buf[i + *extra]     = 'j';
                trg_buf[i + *extra + 1] = 'a';
                break;

            default:
                MY_ASSERT(0 && "UNREACHABLE");
                break;
        }
    }
    else
        MY_ASSERT(0 && "UNREACHABLE");

    return;
}