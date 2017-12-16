/*
Ethan Chen and Edwin Jain 
HW4: Arith 

This file contains the implementation for the functiosn declared 
in rgb_cvc.h 
*/ 

#include "rgb_cvc.h"

/* if number is above or below the range, rounds to nearest number in range*/ 
static float round_to_range (float num, float lo, float hi)
{
        if (num < lo) {
                num = lo;
        } else if (num > hi) {
                num = hi;   
        }
        return num;
}


cvc rgb_to_cvc(Pnm_rgb rgb, unsigned denom)
{
        unsigned int r, g, b;
        float y, pb, pr;
        cvc pixel;
        r = rgb -> red;
        g = rgb -> green;
        b = rgb -> blue;
        y =  (0.299    * r + 0.587    * g + 0.114    * b)/denom;
        pb = (-0.168736 * r - 0.331264 * g + 0.5      * b)/denom;
        pr =  (0.5      * r - 0.418688 * g - 0.081312 * b)/denom;
        /*rounds valued to within specified range */ 
        pixel.y = round_to_range(y, 0, 1);
        pixel.pb = round_to_range(pb, -0.5, 0.5);
        pixel.pr = round_to_range(pr, -0.5, 0.5);
        return pixel;
}


struct Pnm_rgb rgbify (float y, float pb, float pr, int denom)
{
        float r, g, b;
        struct Pnm_rgb pixel;
        /*performs calculations*/ 
        r = 1.0 * y + 0.0      * pb + 1.402    * pr;
        g = 1.0 * y - 0.344136 * pb - 0.714136 * pr;
        b = 1.0 * y + 1.772    * pb + 0.0      * pr;

        /*rounds to within specified range */ 
        r = round_to_range (r, 0.0, 1.0);
        g = round_to_range (g, 0.0, 1.0);
        b = round_to_range (b, 0.0, 1.0);
        /* multiplies by denom to get proper rbg values */ 
        pixel.red = round (r * denom);
        pixel.green = round (g * denom);
        pixel.blue = round (b * denom);
        return pixel;
}




