/*
Ethan Chen and Edwin Jain
HW4: Arith

This file is deals with reading in the pic and trimming the edges 
for compresion, and reading in the header and making sure the UArray_2 
has a blocksize for decompression. 

*/ 

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "except.h"
#include "uarray2b.h"
#include "pnm.h"
#include "a2methods.h"
#include "a2blocked.h"


/* Exception raised if image provided is smaller than 2x2 */
Except_T Less_Than_2x2; 

/* 
        Input: File pointer
        Output: Pnm_ppm 
    This function stores the pictures and returns a Pnm_ppm with blocksize 2.
*/
Pnm_ppm store_pic (FILE *fp);

/*
        Input: File pointer
        Output: Pnm_ppm
    This function reads in the header and returns a Pnm_ppm with the 
    right width, height, and blocksize. 
*/
Pnm_ppm read_header (FILE *fp); 

