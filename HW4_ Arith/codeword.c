#include "codeword.h"


/* 
    This array contains important information about the codeword.
    The 18 indexes represent a, b, c, d, pb and pr (3 indexes for each). 
    every third index starting at 0 indicates which index in our 
    codeword UArray_T this variable should be stored in. 
    Every third index starting at 1 indicates how many bits this variable
    should take up. Every third index starting at 2 indicates whether this 
    variable is signed or unsigned (0 is unsigned, 1 is signed). 

    PLEASE NOTE: WE WERE LURED INTO THIS STUPID IDEA BY THE PROSPECT OF 
    BEING ABLE TO COMPLETE THE CHALLENGE PROBLEM WITH ONE LINE LIKE NORMAN 
    RAMSEY. WE HAVE HAD SEVERAL TA'S LAUGH AND RIDICULE OUR IDEA SO PLEASE 
    BE GENTLE. 
*/ 
const int ORDER[18] = 
        {0, 6, 0, 1, 6, 1, 2, 6, 1,  3, 6, 1, 4, 4, 0, 5, 4, 0 };


void pack_and_print (UArray_T codeword)
{       
        int i, j; 
        uint64_t word = 0; 
        int width = 32;  
        for (i = 0; i < 18; i += 3) {
                if (ORDER[i+2] == 0) { /*is unsigned */ 
                        /* Uses appropriate ORDER index to 
                                determine lsb and width */
                        word = Bitpack_newu(word, ORDER[i+1], 
                                        (width - ORDER[i +1]), 
                        *((unsigned *)(UArray_at (codeword, ORDER[i]))));
                }  else {
                    /* Uses appropriate ORDER index to 
                                determine lsb and width */
                        word = Bitpack_news(word, ORDER[i+1], 
                                        (width - ORDER[i +1]), 
                        *((signed *)(UArray_at (codeword, ORDER[i]))));
                }
                width = width - ORDER[i+1]; 
        }
        for (j = 24; j >= 0; j -= 8) {
            /* Putchar puts codeword in file*/ 
                putchar(Bitpack_getu(word, 8, j));
        }
}

int read_codeword (FILE *fp)
{
        uint64_t word; 
        for (int i = 24; i >= 0; i = i - 8) {
               int c = fgetc (fp);
               if (c == EOF) {
                       return c;
               }
               /* Creates codeword byte by byte */ 
               word = Bitpack_newu(word, 8, i, (uint64_t)c);         
        }
        return (int) word;
}

UArray_T unpack (int word) 
{
        if (word == EOF) {
                return NULL;
        }
        uint64_t codeword = (uint64_t) word;
        unsigned lsb = 32;
        UArray_T array = UArray_new (8, sizeof(signed));
        for (int i = 0; i < 18; i += 3) {
                if (ORDER[i + 2] == 0) { /* if unsigned */ 
                    /* Uses appropriate ORDER index to 
                                determine lsb and width */
                        *((unsigned *)(UArray_at (array, ORDER[i]))) = 
                        Bitpack_getu (codeword, ORDER[i+1], lsb - ORDER[i+1]);
                } else {
                    /* Uses appropriate ORDER index to 
                                determine lsb and width */
                        *((signed *)(UArray_at (array, ORDER[i]))) =
                        Bitpack_gets (codeword, ORDER[i+1], lsb - ORDER[i+1]);
                }
                lsb = lsb - ORDER[i+1];
        }
        return array;
}





