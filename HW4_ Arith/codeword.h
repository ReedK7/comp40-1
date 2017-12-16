#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "uarray.h"
#include <math.h>
#include "pnm.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "arith40.h"
#include "bitpack.h"

/****************************************************************

                        COMPRESSION FUNCTIONS 
                        
****************************************************************/
/*
        Input: UArray_T codeword 
        Output: none 
        Packs codeword using values from UArray_T codeword
        using bitpack, writes to file using putchar. 
*/ 
void pack_and_print (UArray_T codeword);

/* 
        Input: File pointer
        Output: Int
        Uses bitpack to read in a codeword 
        and store it in an int, then returns 
        that int. Uses array ORDER to determine width, LSB
        and which bitpack new to use (signed or unsigned).  

*/  

/****************************************************************

                        DECOMPRESSION FUNCTIONS 
                        
****************************************************************/
int read_codeword (FILE *fp);

/* 
        Input: Int
        Output: UArray_T 
        Uses bitpack to place relevant values 
        into the UArray_T in the order a, b, c, 
        pb and pr. Uses array ORDER to determine 
        when to place a signed or unsigned. 
*/ 
UArray_T unpack (int codeword);





