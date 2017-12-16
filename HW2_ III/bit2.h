/*****************************************************
 * Ethan Chen and Edwin Jain 
 * Homework 2 
 * 
 * This is the header file for the bit2 data structure. 
 * Our Bit2 is a UArray of Bit_T bit vectors, allowing 
 * for a representation of a 2D bitmap. 
 * 
******************************************************/

#ifndef Bit2_INCLUDED
#define Bit2_INCLUDED

#define T Bit2_T
typedef struct T *T;

/*
        Access elements in the Bit2 struct.
        INPUT: A Bit2_T
        OUTPUT: An int
*/ 
extern int Bit2_width (T Bit2);
extern int Bit2_height (T Bit2);


/* 
        Make a new instance of a Bit2_T with the specified dimensions.
        Width and height of 0 result in a CRE.
        INPUT: Two ints, width and height
        OUTPUT: A Bit2_T  
*/ 
extern T Bit2_new (int width, int height);

/* 
        Frees memory allocated for a Bit2_T.
        INPUT: A pointer to a Bit2_T
        OUTPUT: Void
*/
extern void Bit2_free (T *Bit2);

/* 
        Get or put elements in Bit2. While get returns the value at 
        [col, row] in Bit2, put returns the same value and replaces it
        with the specified value. Putting a non-bit value causes a CRE.
        INPUT: A Bit2_T, two ints (index), an int value (for put)
        OUTPUT: An int (specifically a 0 or 1) 
*/ 
extern int Bit2_get (T Bit2, int col, int row);
extern int Bit2_put (T Bit2, int col, int row, int value);


/*
        Functions to map row and column major over a Bit2.
        Allows the client to specify a function (apply) that can be applied
        to every element in a Bit2 row by row or column by column.
        INPUT: A Bit2; an apply function that takes in: col and row 
        indices, the Bit2, the value at [col, row], a void *closure; and
        a void *closure.
        OUTPUT: Voidmap row major and map column major
*/ 
extern void Bit2_map_row_major 
                (T Bit2, 
                void apply (int col, int row, T Bit2, int val, void *cl), 
                void *cl);

extern void Bit2_map_col_major 
                (T Bit2, 
                void apply(int col, int row, T Bit2, int val, void *cl), 
                void *cl);



#undef T
#endif



