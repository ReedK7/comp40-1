#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "compress40.h"
#include "process_image.h"
#include "calculate.h"
#include "codeword.h"
#include "rgb_cvc.h"


static void (*compress_or_decompress)(FILE *input) = compress40;
void compress40 (FILE *fp);
void decompress40 (FILE *fp);
void fill_RGB (int i, int j, A2Methods_UArray2 pixels, void *elem, void *fm);
int end_block (int i, int j);
void process_pixel (int i, int j, A2Methods_UArray2 pixels, void *elem, 
                                                                void *cl); 
Except_T Too_Small = {"Image supplied is smaller than header indicates"};


/* This struct is passed in as a closure during decompression. */
typedef struct file_and_methods {
        A2Methods_T methods;
        int denom;
        FILE *fp;
} file_and_methods;


int main(int argc, char *argv[])
{
        int i;

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-c") == 0) {
                        compress_or_decompress = compress40;
                } else if (strcmp(argv[i], "-d") == 0) {
                        compress_or_decompress = decompress40;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n",
                                argv[0], argv[i]);
                        exit(1);
                } else if (argc - i > 2) {
                        fprintf(stderr, "Usage: %s -d [filename]\n"
                                "       %s -c [filename]\n",
                                argv[0], argv[0]);
                        exit(1);
                } else {
                        break;
                }
        }
        assert(argc - i <= 1);    /* at most one file on command line */
        if (i < argc) {
                FILE *fp = fopen(argv[i], "r");
                assert(fp != NULL);
                compress_or_decompress(fp);
                fclose(fp);
        } else {
                compress_or_decompress(stdin);
        }
}

void compress40 (FILE *fp)
{
        /* Initialize picture and methods by using Process_Image module */ 
        Pnm_ppm pic;
        A2Methods_T methods = uarray2_methods_blocked;
        pic = store_pic (fp);
        pic -> methods = (const A2Methods_T)methods;
        
        /* 
            At positions 0-5, we store values relevant to CVC calculations:
            {y1, y2, y3, y4, pb, pr}, and then {a, b, c, d, pb, pr}.
            At position 6, we store the denominator required for the 
            RGB-CVC conversion. At position 7, we store a counter that
            ranges from 0-4, which tells us which y-value we need to place.
            It also tells us, when it equals 4, that it's time to perform
            calculations and print out a codeword. This array is passed
            in as a closure to our mapping function. 
        */
        UArray_T cl = UArray_new (8, sizeof(unsigned)); 
        *((unsigned *)(UArray_at (cl, 6))) = pic -> denominator;
        
         /* Print out header with correct width and height */ 
         fprintf (stdout, "COMP40 Compressed image format 2\n%u %u\n", 
                pic->width, pic->height);
        
        /* After processing every pixel, we free the array and the ppm */
        methods -> map_block_major (pic -> pixels, process_pixel, &cl);
        UArray_free (&cl);
        Pnm_ppmfree (&pic); 
}

void process_pixel (int i, int j, A2Methods_UArray2 pixels, 
                                void *elem, void *cl) 
{
        /* 
            Don't need indices or the pixels array. We know this menas
            that a small mapping function would have made for cleaner
            code. However, we encountered some bugs when doing that
            and somehow managed to make the larger mapping function
            work. 
        */
        (void) i; 
        (void) j;
        (void) pixels;

        /* Get our UArray, denominator, and index from the closure */
        UArray_T closure = *((UArray_T *) (cl));
        int denom = *((int *) (UArray_at (closure, 6)));
        int index = *((int *)(UArray_at (closure, 7)));

        /* Use RGB-CVC to get a cvc pixel, and place its values in the
            closure. Note that we add the Pb and Pr values, but divide them
            by 4 later in the Calculate module. */
        cvc cvcpixel = rgb_to_cvc ((Pnm_rgb) elem, denom);
        *((float *) (UArray_at (closure, index))) = cvcpixel.y;
        *((float *) (UArray_at (closure, 4))) += cvcpixel.pb;
        *((float *) (UArray_at (closure, 5))) += cvcpixel.pr;

        /* If index is 4, we just performed calculations, so zero out closure.
            If index is not 4, then we increment and place it in closure. */
        if (index == 4) {
            index = 0; 
            *((float *) (UArray_at (closure, 4))) = 0;
            *((float *) (UArray_at (closure, 5))) = 0;
        }
        index++; 
        *((int *)(UArray_at (closure, 7))) = index;

        /* We're ready to perform calculations and print after processing 
                4 pixels. */
        if (index == 4) {
                closure = fit_to_bits (DCT (average_quantize (closure)));
                pack_and_print (closure);
        }
}

void decompress40 (FILE *fp)
{
        /* Initialize picture and closure. The closure holds a file pointer,
            a denominator, and a methods suite. */
        Pnm_ppm pic = read_header (fp);
        file_and_methods *cl = malloc (sizeof(*cl));
        cl -> denom = pic -> denominator;
        cl -> methods = uarray2_methods_blocked;
        cl -> fp = fp;

        /* We map over our empty array block-major and fill in pixels */
        cl -> methods -> map_block_major (pic->pixels, fill_RGB, cl);

        /* Our map function will make the file pointer NULL if there
            weren't enough codewords in the file to match the width
            and height provided. In that case, we raise an exception. */
        if (cl -> fp == NULL) {
                RAISE (Too_Small);
                free (cl);
                Pnm_ppmfree (&pic); 
        } else {
                /* Else, we write to standard output */
                Pnm_ppmwrite (stdout, pic);
                free (cl);
                Pnm_ppmfree (&pic);
        }
}

void fill_RGB (int i, int j, A2Methods_UArray2 pixels, void *elem, void *fm)
{
        /* Extract values from the file_and_methods closure passed in */
        FILE *fp = ((file_and_methods *) fm) -> fp;
        int denom = ((file_and_methods *) fm) -> denom;
        A2Methods_T methods = ((file_and_methods *) fm) -> methods;

        /* If there are still codewords to be read and we're at the 
            end of a block, then we unpack and perform the conversion. */
        if ((fp != NULL) && (end_block (i, j))) {
                UArray_T closure = unpack (read_codeword (fp));

                /* Unpack will return a NULL closure to signal
                    premature EOF */ 
                if (closure == NULL) {
                        fclose (fp);
                        fp = NULL;
                } else {
                       
                        /* Perform inverse calculations and extract 
                            Y, Pb, Pr values from the returned array */ 
                        closure = inverse_DCT (extract_from_bits (closure));
                        float y1 = *((float *)(UArray_at (closure, 0)));
                        float y2 = *((float *)(UArray_at (closure, 1)));
                        float y3 = *((float *)(UArray_at (closure, 2)));
                        float y4 = *((float *)(UArray_at (closure, 3)));
                        float pb = *((float *)(UArray_at (closure, 4)));
                        float pr = *((float *)(UArray_at (closure, 5)));

                        /* RGBify our CVC values and place them in the ppm */
                        *((Pnm_rgb)(methods->at(pixels, i-1, j-1))) 
                                = rgbify (y1, pb, pr, denom);  
                        *((Pnm_rgb)(methods->at(pixels, i, j-1))) 
                                = rgbify (y2, pb, pr, denom);  
                        *((Pnm_rgb)(methods->at(pixels, i-1, j))) 
                                = rgbify (y3, pb, pr, denom);  
                        *((Pnm_rgb)(elem)) 
                                = rgbify (y4, pb, pr, denom); 

                        /* Free the array we've been using */
                        UArray_free(&closure);     
                }
        }
}

/* Tells us whether we're at the end of a block given two indices.
    Note that we're at the end of a block only if i and j are odd.
    By masking the last bit, we can figure out exactly that. */
int end_block (int i, int j)
{
        return ((i &1) && (j & 1));
}

