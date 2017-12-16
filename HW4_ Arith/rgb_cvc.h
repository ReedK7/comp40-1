/*
Ethan Chen and Edwin Jain 
HW4: Arith 

This module deals with converting RGB color space
to component video space
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


/* 
        struct CVC containing a 
        y value, pb value
        and pr value. 
*/ 

typedef struct cvc {
        float y;
        float pb;
        float pr;
} cvc;

/****************************************************************

                        COMPRESSION FUNCTIONS 
                        
****************************************************************/

/* 
        input: Pnm_rgb and a unsigned denominator 
        output: struct cvc 
        Performs the calculations to to turn 
        a pixel from rgb color space to cvc color space. 
        Returns a struct cvc. 
*/ 
cvc rgb_to_cvc(Pnm_rgb rgb, unsigned denom);


/****************************************************************

                        DECOMPRESSION FUNCTIONS 
                        
****************************************************************/
/*  
        input: 3 floats and an int
        output: Pnm_rbg struct 
        performs calculations turning a CVC color space 
        to a Pnm_rgb struct, then returns Pnm_rgb struct. 
*/ 
struct Pnm_rgb rgbify (float y, float pb, float pr, int denom);
