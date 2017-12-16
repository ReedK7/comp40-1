/**************************************************
*                       Ethan Chen and Edwin Jain
*                               HW3: Locality 
*
*               This is our a2plain implementation. The functions 
*               declarations are located in a provided
*               file called a2plain.h. Our A2plain implementation
*               uses functions from the UArray2 implementation provided. 
*
*
*
***************************************************/


#include <string.h>
#include "a2plain.h"
#include "uarray2.h"


// define a private version of each function in A2Methods_T that we implement
typedef A2Methods_UArray2 A2;   // private abbreviation


/*  
        These create new A2s using the plain UArray2 implementation 
        Inputs: width, height, size
        Output: An A2 instance 
*/ 

static A2 new(int width, int height, int size) {
        return UArray2_new(width, height, size);
}

static A2 new_with_blocksize(int width, int height, int size, int blocksize)
{
        (void)blocksize; 
        return UArray2_new(width, height, size);
}

/*
        These functions acces the blocksize, size, 
        width and height of a A2 Array2
        Input: an A2 
        Output: an int
*/
static int blocksize(A2 array2)
{
        (void)array2;
        return 1;
}

static int width(A2 array2) {
        return UArray2_width(array2); 
}

static int height(A2 array2) {
        return UArray2_height(array2); 
}

static int size(A2 array2) {
        return UArray2_size(array2);
}

/*
        This function frees the memory allocated for the A2
        Input: An A2 pointer 
        Output: Nothing
*/ 
static void a2free(A2 * array2p)
{
        UArray2_free((UArray2_T *)array2p); 
}

/* 
        Find and returns a pointer to the object at position [i,j] in the A2
        Input: A2, column index, row index
        Output: An A2Methods_Object pointer
*/ 
static A2Methods_Object *at(A2 array2, int i, int j) {
        return UArray2_at(array2, i, j); 
}



/* 
        The apply function
        Input: Ints for [col, row], a UArray2_T, 
        pointer to an element at [col, row], closure
        Output: None
*/ 
typedef void applyfun(int i, int j, UArray2_T array2b, void *elem, void *cl);

/*
        These are the mapping functions which map over an A2 in row-major 
        and column-major order 
        Input: An A2, an apply function and a closure
        Output: None 
*/
static void map_row_major(A2 array2, A2Methods_applyfun apply, void *cl) {
        UArray2_map_row_major(array2, (applyfun *) apply, cl); 
}

static void map_col_major(A2 array2, A2Methods_applyfun apply, void *cl) {
        UArray2_map_col_major(array2, (applyfun *) apply, cl); 
}

struct small_closure {
        A2Methods_smallapplyfun *apply;
        void *cl;
};

static void apply_small(int i, int j, UArray2_T array2, void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)array2;
        cl->apply(elem, cl->cl);
}


/* 
        mapping function that expects a smaller apply function
*/ 
static void small_map_row_major(A2 a2, A2Methods_smallapplyfun apply,
                                  void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}

static void small_map_col_major(A2 a2, A2Methods_smallapplyfun apply,
                                  void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}

/* The method suite */
static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize,
        a2free,
        width,
        height,
        size,
        blocksize,
        at,
        map_row_major,                  
        map_col_major,
        NULL,                   //map_block_major
        map_row_major,  // map_default
        small_map_row_major,                    
        small_map_col_major,
        NULL,   //small_map_block_major
        small_map_row_major,    // small_map_default
};

// finally the payoff: here is the exported pointer to the struct

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
