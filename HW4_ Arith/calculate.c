/*
Ethan Chen and Edwin Jain 
HW4: Arith 

This is the implementations of the functions 
declared in the calculate.h file. 

*/ 
#include "calculate.h"



UArray_T average_quantize (UArray_T four_cvcs)
{
        float pb = *((float *) (UArray_at(four_cvcs, 4)));
        float pr = *((float *) (UArray_at(four_cvcs, 5)));
        pb /= 4.0;
        pr /= 4.0;
        unsigned new_pb = Arith40_index_of_chroma(pb);
        unsigned new_pr = Arith40_index_of_chroma(pr);
        *((unsigned *)(UArray_at(four_cvcs, 4))) = new_pb;      
        *((unsigned *)(UArray_at(four_cvcs, 5))) = new_pr;
        return four_cvcs;
}

UArray_T DCT(UArray_T four_cvcs)
{
        float y1, y2, y3, y4, a, b, c, d;
        y1 = *((float *) (UArray_at(four_cvcs, 0)));
        y2 = *((float *) (UArray_at(four_cvcs, 1)));
        y3 = *((float *) (UArray_at(four_cvcs, 2)));
        y4 = *((float *) (UArray_at(four_cvcs, 3)));
        a = (y4 + y3 + y2 + y1) / 4.0;
        b = (y4 + y3 - y2 - y1) / 4.0;
        c = (y4 - y3 + y2 - y1) / 4.0;
        d = (y4 - y3 - y2 + y1) / 4.0;
        *((float *)(UArray_at(four_cvcs, 0))) = a;      
        *((float *)(UArray_at(four_cvcs, 1))) = b;
        *((float *)(UArray_at(four_cvcs, 2))) = c;      
        *((float *)(UArray_at(four_cvcs, 3))) = d;
        return four_cvcs;
} 

/*  
    Input: float 
    Output: signed 
    Quantize helper function, takes in a float
    and quantizes it. 
*/ 
static signed quantize (float x)
{
        if (fabs (x) > 0.3) {
                x = ((x > 0) - (x < 0)) * 0.3;
        }
        return round ((x * 104) - 0.5);
}




UArray_T fit_to_bits (UArray_T four_cvcs)
{       
        float a = *((float *)(UArray_at(four_cvcs, 0))); 
        unsigned new_a = round (a * 63); 
        *((unsigned *) (UArray_at(four_cvcs, 0))) = new_a;

        *((signed *) (UArray_at (four_cvcs, 1))) = 
                quantize (*((float *)(UArray_at(four_cvcs, 1))));
        *((signed *) (UArray_at (four_cvcs, 2))) = 
                quantize (*((float *)(UArray_at(four_cvcs, 2))));
        *((signed *) (UArray_at (four_cvcs, 3))) = 
                quantize (*((float *)(UArray_at(four_cvcs, 3))));
        return four_cvcs;
}

/* 
    Input: signed
    Output: Float 
    Reverses the quantization calculation 
    performed in compression. 
*/ 
static float reverse_quantize (signed x)
{
        return (x + 0.5) / (104);
}

UArray_T extract_from_bits (UArray_T codeword)
{

        *((float *) (UArray_at (codeword, 0))) = 
                (*((unsigned *) (UArray_at (codeword, 0))))/63.0f;
        *((float *) (UArray_at (codeword, 1))) = 
                reverse_quantize(*((signed *) (UArray_at (codeword, 1))));
        *((float *) (UArray_at (codeword, 2))) = 
                reverse_quantize(*((signed *) (UArray_at (codeword, 2))));
        *((float *) (UArray_at (codeword, 3))) = 
                reverse_quantize(*((signed *) (UArray_at (codeword, 3))));

        *((float *) (UArray_at (codeword, 4))) = 
                Arith40_chroma_of_index ( 
                        *((unsigned *) (UArray_at (codeword, 4))));
        *((float *) (UArray_at (codeword, 5))) = 
                Arith40_chroma_of_index (
                        *((unsigned *) (UArray_at (codeword, 5))));
        return codeword;
}


UArray_T inverse_DCT (UArray_T codeword)
{
        float a, b, c, d, y1, y2, y3, y4; 
        a = *((float *)(UArray_at (codeword, 0)));
        b = *((float *)(UArray_at (codeword, 1)));
        c = *((float *)(UArray_at (codeword, 2)));
        d = *((float *)(UArray_at (codeword, 3)));
        y1 = a - b - c + d;
        y2 = a - b + c - d;
        y3 = a + b - c - d;
        y4 = a + b + c + d;
        *((float *)(UArray_at (codeword, 0))) = y1;
        *((float *)(UArray_at (codeword, 1))) = y2;
        *((float *)(UArray_at (codeword, 2))) = y3;
        *((float *)(UArray_at (codeword, 3))) = y4;
        return codeword;
}

