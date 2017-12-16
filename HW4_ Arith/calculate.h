/*
Ethan Chen and Edwin Jain 
HW4: Arith 

This is the header file for our calculations module, 
which perfoms calculations such as averaging and quantizing 
the RGB structs, performing the discrete cosine tranformation
and the inverse, as well as fitting to a certain number of bits

*/ 


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
    Input: UArray_T 
    Output: UArray_T
    averages and quantizes the pb and pr values
*/ 
UArray_T average_quantize (UArray_T four_cvcs);

/*
    Input: UArray_T 
    Output: UArray_T 
    Performs the discrete cosine transformation 
    on the y1, y2, y3 and y4 values and places a, b, c, and d
    in the correct place in the UArray_T four_cvcs, 
    then returns four_cvcs.
*/ 
UArray_T DCT(UArray_T four_cvcs); 

/*      
        Input: UArray_T 
    Output: UArray_T 
    Takes in UArray_T four_cvcs, 
    fits a to 9 bits, 
    and fits b, c and d into 5 bits by calling 
    quantize bits and returns four_cvcs with new values. 
*/
UArray_T fit_to_bits(UArray_T four_cvcs);



/****************************************************************

                        DECOMPRESSION FUNCTIONS 
                        
****************************************************************/

/*
    Input: UArray_T 
    Output: UArray_T 
    Applies reverse quantize to b, c, and d, 
    and extracts correct value out of a. 
    Applies Arith40_chroma_of_index to pb and pr 
    Returns Uarray_T codeword with altered values. 
*/ 


UArray_T extract_from_bits (UArray_T codeword);

/* 
    Input: UArray_T 
    Output: UArray_T 
    Reverses the DCT calculations 
    done in compression and returns 
    new UArray_T with y1, y2, y3 and y4
    instead of a, b, c, d. 
*/ 
UArray_T inverse_DCT (UArray_T codeword);
















