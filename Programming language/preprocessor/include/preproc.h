#ifndef PREPROC_H
    #define PREPROC_H

    enum RusLetters1st{
        RUS_LETTERS_LOW_AA = -80,
        RUS_LETTERS_LOW_B  = -79,
        RUS_LETTERS_LOW_V  = -78,
        RUS_LETTERS_LOW_G  = -77,
        RUS_LETTERS_LOW_D  = -76,
        RUS_LETTERS_LOW_EE = -75,
        RUS_LETTERS_LOW_JJ = -74,
        RUS_LETTERS_LOW_Z  = -73,
        RUS_LETTERS_LOW_II = -72,
        RUS_LETTERS_LOW_IJ = -71,
        RUS_LETTERS_LOW_K  = -70,
        RUS_LETTERS_LOW_L  = -69,
        RUS_LETTERS_LOW_MM = -68,
        RUS_LETTERS_LOW_N  = -67,
        RUS_LETTERS_LOW_OO = -66,
        RUS_LETTERS_LOW_P  = -65,

        RUS_LETTERS_UPP_AA = -112,
        RUS_LETTERS_UPP_B  = -111,
        RUS_LETTERS_UPP_V  = -110,
        RUS_LETTERS_UPP_G  = -109,
        RUS_LETTERS_UPP_D  = -108,
        RUS_LETTERS_UPP_EE = -107,
        RUS_LETTERS_UPP_JO = -127,
        RUS_LETTERS_UPP_JJ = -106,
        RUS_LETTERS_UPP_Z  = -105,
        RUS_LETTERS_UPP_II = -104,
        RUS_LETTERS_UPP_IJ = -103,
        RUS_LETTERS_UPP_K  = -102,
        RUS_LETTERS_UPP_L  = -101,
        RUS_LETTERS_UPP_MM = -100,
        RUS_LETTERS_UPP_N  = -99,
        RUS_LETTERS_UPP_OO = -98,
        RUS_LETTERS_UPP_P  = -97,
        RUS_LETTERS_UPP_R  = -96,
        RUS_LETTERS_UPP_SS = -95,
        RUS_LETTERS_UPP_TT = -94,
        RUS_LETTERS_UPP_Y  = -93,
        RUS_LETTERS_UPP_F  = -92,
        RUS_LETTERS_UPP_HH = -91,
        RUS_LETTERS_UPP_TS = -90,
        RUS_LETTERS_UPP_CH = -89,
        RUS_LETTERS_UPP_SH = -88,
        RUS_LETTERS_UPP_SC = -87,
        RUS_LETTERS_UPP_TW = -86,
        RUS_LETTERS_UPP_IH = -85,
        RUS_LETTERS_UPP_MA = -84,
        RUS_LETTERS_UPP_JE = -83,
        RUS_LETTERS_UPP_U  = -82,
        RUS_LETTERS_UPP_JA = -81,
    };

    enum RusLetters2nd {
        RUS_LETTERS_LOW_JO = -111,
        RUS_LETTERS_LOW_R  = -128,
        RUS_LETTERS_LOW_SS = -127,
        RUS_LETTERS_LOW_TT = -126,
        RUS_LETTERS_LOW_Y  = -125,
        RUS_LETTERS_LOW_F  = -124,
        RUS_LETTERS_LOW_HH = -123,
        RUS_LETTERS_LOW_TS = -122,
        RUS_LETTERS_LOW_CH = -121,
        RUS_LETTERS_LOW_SH = -120,
        RUS_LETTERS_LOW_SC = -119,
        RUS_LETTERS_LOW_TW = -118,
        RUS_LETTERS_LOW_IH = -117,
        RUS_LETTERS_LOW_MA = -116,
        RUS_LETTERS_LOW_JE = -115,
        RUS_LETTERS_LOW_U  = -114,
        RUS_LETTERS_LOW_JA = -113,
    };

    enum RusLetterSigns {
        RUS_LETTER_SIGNS_FIRST =  -48,
        RUS_LETTER_SIGNS_SECOND = -47,
    };

    void preprocessing(char * src_buf, char * trg_buf,
                       long src_buf_size, long trg_buf_size);

    const double EXPAND_BUFFER_COEFFICIENT = 1.5;
    extern const char * PREPROCESSED_PROG_FILE;

#endif // PREPROC_H