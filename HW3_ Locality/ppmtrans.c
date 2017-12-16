/************************************************
*                       HW3: Locality                               *
*               Ethan Chen and Edwin Jain              *
*----------------------------------------------------------*
*       This program allows users to rotate       
*       images 0, 90, or 180 degrees and specify
*       which mapping to use. They can also time
*       how long it takes to do the transformation.
************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cputiming.h"
#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (0)

/*Struct to be passed in as closure to the map function. This allows us
*  to access a second UArray2 and the method suite*/ 
typedef struct array_and_methods {
        A2Methods_UArray2 array; 
        A2Methods_T methods; 

} array_and_methods; 

/* Rotate functions */ 
void rotate_90(int col, int row, A2Methods_UArray2 array2, 
                            void *elem, void *cl);
void rotate_180 (int col, int row, A2Methods_UArray2 array2, 
                            void *elem, void *cl);
void rotate_270(int col, int row, A2Methods_UArray2 array2, 
                            void *elem, void *cl);

/* Give the user a message about how to use the program correctly */
static void usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}

int main(int argc, char *argv[]) 
{
        char *time_file_name = NULL;
        (void) time_file_name;
        int   rotation       = 0;
        int   i;
        CPUTime_T timer; 
        double time_used; 
        (void) time_used;
        FILE *fp;
        FILE *time;

        /* Default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);

        /* Default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map);

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i];
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        break;
                }
        }

        /* Determines whether file is in command line and 
                opens it if needed*/
        if (argc != i) {
                fp = fopen(argv[i], "rd");
        } else { 
                /* If no file is provided, get from stdin*/ 
                fp = (stdin);    
        }
        if (fp == NULL) {
                fprintf(stderr, "Error in reading from file\n");
                exit(EXIT_FAILURE); 
        }
        
        /* Reads pixels into A2 */ 
        Pnm_ppm pixmap = Pnm_ppmread (fp, methods);
        int width, height; 

        if (rotation == 0) { /*if rotation is 0 just returns original image*/ 
                Pnm_ppmwrite(stdout, pixmap);
                Pnm_ppmfree(&pixmap); 
                if (argc != i) {
                        fclose(fp);
                }
                exit(EXIT_SUCCESS);

        } else if (rotation == 90 || rotation == 270) { 
                width = methods -> height(pixmap -> pixels); 
                height = methods -> width(pixmap -> pixels);
        /*sets dimensions for image to be returned depending on rotation*/ 
        } else if (rotation == 180) {
                width = methods -> width(pixmap -> pixels); 
                height = methods -> height(pixmap -> pixels);  
        } 

        /*initializes new UArray2 or UArray2b to be returned in rotated form*/ 
        A2Methods_UArray2 rotated = (methods -> new (width, height, 
                                        (sizeof (struct Pnm_rgb)))); 

        array_and_methods *closure = malloc(sizeof(*closure));
        closure -> array = rotated;
        closure -> methods = methods;  

        /*initializes timer*/ 
        timer = CPUTime_New();
        if (rotation == 90) {
                CPUTime_Start(timer);
                map(pixmap -> pixels, rotate_90, closure); 
                time_used = CPUTime_Stop(timer);
        } else if (rotation == 180) {
                CPUTime_Start(timer);
                map(pixmap -> pixels, rotate_180, closure); 
                time_used = CPUTime_Stop(timer);
        } else if (rotation == 270) {
                CPUTime_Start(timer);
                map(pixmap -> pixels, rotate_270, closure); 
                time_used = CPUTime_Stop(timer);
        }

        /*opens file to write in timing results to and writes to file*/ 
        if (time_file_name != NULL) {
                time = fopen(time_file_name, "w"); 
                if (time != NULL){
                        fprintf(time, 
                                "Rotation performed in %f nanoseconds\n",
                                 time_used);
                        fprintf(time, "time per pixel was %f nanoseconds\n", 
                                    time_used/(width*height));
                }
                else {
                        exit(EXIT_FAILURE);
                }
        }

        /*frees old image*/ 
        methods -> free (& (pixmap->pixels));

        /*assigns new image to the Pnm_ppm struct */ 
        pixmap -> pixels = rotated; 
        pixmap -> width = width; 
        pixmap -> height = height;
        Pnm_ppmwrite (stdout, pixmap);  /*prints out new image*/ 
        /*frees all memory and closes file if necessary*/ 
        Pnm_ppmfree(&pixmap); 
        free(closure);
        CPUTime_Free(&timer);
        if (argc != i) {
                fclose(fp);
        }
        exit(EXIT_SUCCESS);
}

/************************************************
* These functions rotate an image a specified amount of
* degrees. They take in two ints [col, row], an A2, a pointer
* to an element in A2 at the current index, and a closure that
* contains another A2 and a method suite. They return nothing.
*-------------------------------------------------------------*/

void rotate_90(int col, int row, 
                        A2Methods_UArray2 array2, 
                        void *elem, void *cl) { 
        int array2height, new_col;
        A2Methods_T methods = ((array_and_methods *)cl) -> methods;
        A2Methods_UArray2 new_array = ((array_and_methods *)cl) -> array;
        array2height = methods -> height(array2); 
        new_col = array2height - row - 1; 
        /* Places pixel in rotated position in new pixelmap */ 
        *((Pnm_rgb)(methods -> at (new_array, new_col, col))) = 
        *((Pnm_rgb)elem); 
} 

void rotate_180 (int col, int row, 
                        A2Methods_UArray2 array2, 
                        void *elem, void *cl) {
        int array2height, array2width, new_col, new_row; 
        A2Methods_T methods = ((array_and_methods *)cl) -> methods;
        A2Methods_UArray2 new_array = ((array_and_methods *)cl) -> array;
        array2height = methods -> height(array2); 
        array2width = methods -> width(array2); 
        new_col = array2width - col - 1;
        new_row = array2height - row - 1; 
        /* Places pixel in rotated position in new pixelmap*/ 
        *((Pnm_rgb)(methods -> at (new_array, new_col, new_row))) = 
        *((Pnm_rgb)elem);
}

void rotate_270(int col, int row, 
                        A2Methods_UArray2 array2, 
                        void *elem, void *cl) { 
        int array2width, new_row;
        A2Methods_T methods = ((array_and_methods *)cl) -> methods;
        A2Methods_UArray2 new_array = ((array_and_methods *)cl) -> array;
        array2width = methods -> width(array2); 
        new_row = array2width - col - 1; 
        /* Places pixel in rotated position in new pixelmap */ 
        *((Pnm_rgb)(methods -> at (new_array, row, new_row))) = 
        *((Pnm_rgb)elem); 

}


/*************************************************/
