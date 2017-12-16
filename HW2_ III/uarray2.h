/*******************************************************
 * Ethan Chen and Edwin Jain 
 * Homework 2 
 * 
 * This is the header file for our UArray2 implemenation. 
 * Our UArray2 is a UArray filled with UArrays.
 
********************************************************/

#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED

#define T UArray2_T
typedef struct T *T;

/*
        Access elements in the UArray2 struct
        INPUT: A UArray2
        OUTPUT: An int                                        
*/ 
extern int UArray2_width (T UArray2); 
extern int UArray2_height (T UArray2);
extern int UArray2_size (T UArray2);

/* 
        Creates an instance of UArray2.
        Width, height, size cannot be 0.  
        INPUT: Ints specifying width, height, and size 
        OUTPUT: A UArray2 _T                                          
*/ 
extern T UArray2_new (int width, int height, int size);

/* 
        Frees memory allocated for a UArray2
        INPUT: A pointer to a UArray2 type
        OUTPUT: Void
*/
extern void UArray2_free (T *UArray2);

/* 
        Access elements in the UArray2 by specifying the index
        INPUT: A UArray2, and two ints for column and row
        OUTPUT: A pointer to the element at [col, row] in UArray2 
*/ 
extern void *UArray2_at (T UArray2, int col, int row);

/* 
        Functions to map row and column major over a UArray2.
        Allows the client to specify a function (apply) that can be applied
        to every element in a UArray2 row by row or column by column.
        INPUT: A UArray2; an apply function that takes in: col and row 
        indices, the UArray2, the value at [col, row], a void *closure; and
        a void *closure.
        OUTPUT: Void        
*/ 
extern void UArray2_map_row_major 
                (T UArray2, 
                void apply (int col, int row, T UArray2, void *val, void *cl), 
                void *cl);

extern void UArray2_map_col_major 
                (T UArray2, 
                void apply(int col, int row, T UArray2, void *val, void *cl), 
                void *cl);



#undef T
#endif
