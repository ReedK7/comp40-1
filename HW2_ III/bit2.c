#include "bit2.h"
#include "bit.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "except.h"
#include <uarray.h>


#define T Bit2_T

Except_T Init_Zero_E = {"Cannot create a 2D Array with 0 rows or columns"};
Except_T Malloc_E = {"Error while allocating memory"}; 

const int Byte_Size = sizeof(int); 

struct T {
        int width;  /* number of columns, at least 0 */
        int height;  /* number of rows, at least 0 */
        UArray_T outer; /*outer array to hold row arrays */ 
};


 int Bit2_width (T Bit2)
 {
        return Bit2 -> width;
 }

 int Bit2_height (T Bit2) 
 {
        return Bit2 -> height; 
 }

 T Bit2_new (int width, int height) 
{
        if (width == 0 || height == 0) {
                RAISE(Init_Zero_E);
                exit(EXIT_FAILURE);
        }

        T Bit2 = malloc (sizeof (struct T)); 
        if (Bit2 == NULL) {
                RAISE(Malloc_E);
                exit(EXIT_FAILURE);
        }
        /*initializes outer array */
        UArray_T outer = UArray_new (width, sizeof(Bit_T));
        /*fills outer array with bit vectors*/ 
        for (int i = 0; i < width; i++) {
                *(Bit_T *)(UArray_at (outer, i)) = Bit_new (height);
        }

        /* Initialize Bit2 struct */
        Bit2 -> width = width;
        Bit2 -> height = height;
        Bit2 -> outer = outer;

        return Bit2;
}

 void Bit2_free (T *Bit2)
 {
         /* Free the inner bit vectors */
        for (int i = 0; i < (*Bit2) -> width; i++) {
                Bit_free (UArray_at(((*Bit2) -> outer), i));
        }
        /* Free the outer UArray and the Bit2 struct */
        UArray_free (&((*Bit2) -> outer));
        free (*Bit2);
 }

 int Bit2_get (T Bit2, int col, int row)
 {     
        Bit_T *inner = (Bit_T *)(UArray_at ((Bit2 -> outer), col));
        return Bit_get(*inner, row); 
 }

 int Bit2_put(T Bit2, int col, int row, int value) 
 {      
        Bit_T *inner = (Bit_T *)(UArray_at(Bit2 -> outer, col)); 
        return Bit_put(*inner, row, value);
 }

 void Bit2_map_col_major 
                (T Bit2, 
                void apply(int col, int row, T Bit2, int val, void *cl), 
                void *cl)
{
        for (int j = 0; j < Bit2 -> width; j++) {
                for (int i = 0; i < Bit2 -> height; i++) {
                        apply (j, i, Bit2, Bit2_get(Bit2, j, i), cl); 
                } 
        }
}

 void Bit2_map_row_major 
                (T Bit2, 
                void apply (int col, int row, T Bit2, int val, void *cl), 
                void *cl)
{       
        for (int j = 0; j < Bit2 -> height; j++) {
                for (int i = 0; i < Bit2 -> width; i++) {
                        apply(i, j, Bit2, Bit2_get(Bit2, i, j), cl);  

                }
        }
}

                    



