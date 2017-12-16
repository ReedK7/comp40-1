 
#include "uarray2.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "except.h"
#include <uarray.h>

#define T UArray2_T

Except_T Init_Zero_E = {"Cannot create a 2D Array with 0 rows or columns"};
Except_T Malloc_E = {"Error while allocating memory"}; 


struct T {
        int width;  /* number of columns, at least 0 */
        int height;  /* number of rows, at least 0 */
        int size;  /* number of bytes in one element */
        UArray_T outer; /*outer array to hold row arrays */ 
};

 int UArray2_width (T UArray2)
 {
        return UArray2 -> width;
 }

 int UArray2_height (T UArray2) 
 {
        return UArray2 -> height; 
 }

 int UArray2_size (T UArray2)
 {
        return UArray2 -> size;  
 }

 T UArray2_new (int width, int height, int size) 
{
        if (width == 0 || height == 0 || size == 0) {
                RAISE(Init_Zero_E);
                exit(EXIT_FAILURE);
        }

        T UArray2 = malloc (sizeof (struct T)); 
        if (UArray2 == NULL) {
                RAISE(Malloc_E);
                exit(EXIT_FAILURE);
        }
        UArray_T outer = UArray_new (width, height*size);

        for (int i = 0; i < width; i++) {
                *(UArray_T *)(UArray_at (outer, i)) = 
                UArray_new (height, size);
        }

        UArray2 -> width = width;
        UArray2 -> height = height;
        UArray2 -> size = size;
        UArray2 -> outer = outer;

        return UArray2;
}

 void UArray2_free (T *UArray2)
 {
        /* First we free the inner UArrays */
        for (int i = 0; i < (*UArray2) -> width; i++) {
                UArray_free (UArray_at (((*UArray2) -> outer), i));
        }
        /* Then the outer array and the struct are freed */
        UArray_free (&((*UArray2) -> outer));
        free (*UArray2);
 }

 void *UArray2_at (T UArray2, int col, int row)
 {     
        UArray_T *inner= (UArray_T *)(UArray_at ((UArray2 -> outer), col));
        return UArray_at (*inner, row);
 }

 void UArray2_map_col_major 
                (T UArray2, 
                void apply(int col, int row, T UArray2, void *val, void *cl), 
                void *cl)
{
        for (int j = 0; j < UArray2 -> width; j++) {
                for (int i = 0; i < UArray2 -> height; i++) {
                        apply(j, i, UArray2, UArray2_at(UArray2, j, i), cl); 
                } 
        }
}

 void UArray2_map_row_major 
                (T UArray2, 
                void apply (int col, int row, T UArray2, void *val, void *cl),
                void *cl)
{       
        for (int j = 0; j < UArray2 -> height; j++) {
                for (int i = 0; i < UArray2 -> width; i++) {
                        apply(i, j, UArray2, UArray2_at(UArray2, i, j), cl);
                }
        }
}