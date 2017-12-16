/*
Ethan Chen and Edwin Jain 
HW4: Arith 

This is the implementations of the functions 
declared in the process_image.h file. 

*/ 
#include "process_image.h"


Except_T Less_Than_2x2 = {"Picture is too small to compress"};


Pnm_ppm store_pic (FILE *fp)
{
        unsigned i, j, width, height; 
        A2Methods_T methods = uarray2_methods_blocked;

        /* Read in a pixmap, and allocate a new pixmap */
        Pnm_ppm newpixmap = malloc(sizeof(struct Pnm_ppm)); 
        Pnm_ppm pixmap = Pnm_ppmread(fp, methods);

        /* Trim the width and height if odd */
        width = (pixmap -> width) - ((pixmap->width) & 1); 
        height = (pixmap -> height) - ((pixmap->height) & 1);

        /* If the picture was less than 2x2 to begin with, 
                raise an exception. */
        if (width == 0 || height == 0) {
                RAISE (Less_Than_2x2);
                Pnm_ppmfree(&pixmap);
                Pnm_ppmfree (&newpixmap);
                exit (EXIT_FAILURE);  
        }
        /* Initialize fields of newpixmap, including an array with
                blocksize 2. This makes sure our mapping is block-major. */
        newpixmap -> methods = (const A2Methods_T) methods;
        newpixmap -> width = width; 
        newpixmap -> height = height; 
        newpixmap -> denominator = pixmap -> denominator;
        newpixmap -> pixels = methods ->
        new_with_blocksize(width, height, sizeof(struct Pnm_rgb), 2);
        
        /* Place pixels from the old pixmap into newpixmap */
        for (i = 0; i < width; i++) {
                for (j = 0; j < height; j++) {
                        *((struct Pnm_rgb *)
                                (methods -> at (newpixmap -> pixels, i, j))) =
                        *((struct Pnm_rgb *)
                                (methods -> at (pixmap -> pixels, i, j)));
                }
        }
        /* Free the old image and return the new one */
        Pnm_ppmfree(&pixmap);
        return newpixmap;
}

Pnm_ppm read_header (FILE *fp)
{
        unsigned height, width, denominator;

        /* Read in the header */
        int read = fscanf(fp, "COMP40 Compressed image format 2\n%u %u",
                                 &width, &height);
        assert(read == 2);
        int c = getc(fp);
        assert(c == '\n');

        /* Initialize fields of an empty Pnm_ppm that we'll fill */
        A2Methods_T methods = uarray2_methods_blocked;
        A2Methods_UArray2 pixels = methods -> new_with_blocksize
                        (width, height, sizeof (struct Pnm_rgb), 2);
        denominator = 65000;
        Pnm_ppm pixmap = malloc (sizeof(*pixmap));
        assert(pixmap != NULL); 
        struct Pnm_ppm pic = { .width = width, .height = height, 
                                .denominator = denominator, .pixels = pixels,
                                .methods = methods};
        *pixmap = pic;

        return pixmap;
}