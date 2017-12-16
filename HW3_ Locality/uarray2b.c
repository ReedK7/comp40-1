/************************************************
*                       HW3: Locality                               *
*                Ethan Chen and Edwin Jain             *
-------------------------------------------------------------
*       This module implements the blocked
*       version of a UArray2, meaning that it traverses
*       all the cells within a block before visiting 
*       the next block, which is in row-major (English)
*       order. We implement the entire array as a 
*       one-dimensional array. Details of the conversion
*       are documented below.                      
*------------------------------------------------------------
*/


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "assert.h"
#include "except.h"
#include "uarray.h"
#include "uarray2b.h"
#define T UArray2b_T
Except_T Size_zero = 
        {"Tried to initialize with width, height, size or blocksize 0"}; 
/*
* new blocked 2d array
* blocksize = square root of # of cells in block.
* blocksize < 1 is a checked runtime error
*/
struct T {

        UArray_T blocks; /* A single UArray to store our UArray2b */
        int width, height, blocksize, size;  /* Dimensions */
       
};

typedef struct location {
        int col;
        int row;
} location;

/* Takes in ints for the dimensions and returns a UArray2b */
T UArray2b_new (int width, int height, int size, int blocksize)
{
        /* Dimensions cannot be 0 */
        if (width == 0 || height == 0 || size == 0 || blocksize == 0) {
        RAISE (Size_zero); 
        } 
        /* Find out how many blocks fit in the width/height */
        int blocks_width = (width + blocksize - 1)/blocksize;
        int blocks_height = (height + blocksize - 1)/blocksize;
        UArray_T blocks;
        /* Create an array of the right size */
        blocks = UArray_new(blocks_height * blocks_width 
                                                * blocksize * blocksize, size);
        /* Allocate memory for and initialize the UArray2b struct */
        T block_array = malloc (sizeof(UArray_T) + (4 * sizeof(int)));
        (block_array) -> blocks = blocks;
        (block_array) -> width = width;
        (block_array) -> height = height;
        (block_array) -> size = size;
        (block_array) -> blocksize = blocksize; 
        return block_array;
}

const int MAX_BLOCK = 64000;  
/*
* new blocked 2d array: blocksize as large as possible provided
* block occupies at most 64KB (if possible)
* Takes ints for dimensions, except blocksize, and returns a
* UArray2b with the right blocksize to fit a block into 64KB.
*/
T UArray2b_new_64K_block(int width, int height, int size)
{
        int blocksize;
        int num_elements = MAX_BLOCK / size;
        if (num_elements == 0) {
                /* If size > MAX_BLOCK, then blocksize is 1 */
                blocksize = 1;
        } else {
                /* Otherwise, we take the floor of the sqrt of num_elements */
                blocksize = (int) floor (sqrt ((double) num_elements));
        }
        T array = UArray2b_new(width, height, size, blocksize);
        return array; 
}

/*      Frees the array pointed to by array2b           */
void UArray2b_free (T *array2b)
{
        assert (array2b != NULL);
        assert ((*array2b) != NULL);
        UArray_free (&((*array2b) -> blocks));
        free (*array2b);
}

/********************************************************
*      The following functions take in an array2b
*       and return some integer dimension in the struct.
*--------------------------------------------------------------------*/

int UArray2b_width (T array2b)
{
        assert (array2b != NULL);
        return array2b -> width;
}

int UArray2b_height (T array2b)
{
        assert (array2b != NULL);
        return array2b -> height;
}

int UArray2b_size (T array2b)
{
        assert (array2b != NULL);
        return array2b -> size;
}

int UArray2b_blocksize(T array2b)
{
        assert (array2b != NULL);
        return array2b -> blocksize;
}
/*******************************************************
* The following functions convert one set of indices to 
* another. In the conversion to 1D, we take in two ints
* (col, row) along with blocksize and width, and return an int.
* In the conversion to 2D, we just take in an int index with 
* blocksize and width, and return a location struct with [col, row].
* More detail on the calculations is given inside the functions.
*--------------------------------------------------------------------*/  

int convert_to_1D (int col, int row, int blocksize, int width)
{
        int block_col_index, block_row_index, block_num;
        int block, c, r, within_block_index, index;
        /* Find the [col, row] index of the block relative to 
        other blocks */
        block_col_index = col/blocksize; 
        block_row_index = row/blocksize;
        block_num = (width + blocksize - 1)/blocksize;
        /* Find 1D index of block */
        block = block_col_index + (block_num * block_row_index); 
        /* Find 2D index of element relative to its block */
        c = col%blocksize;
        r = row%blocksize;
        /* Find the 1D index of the element relative to the 
        block it’s in */
        within_block_index  = c + (r*blocksize); 
        /* Find and return final 1D index of element */
        index = within_block_index + (block * blocksize * blocksize); 
        return index;
}

location convert_to_2D (int index, int blocksize, int width)
{
        int block, within_index, within_col, within_row;
        int block_num, block_col, block_row, index_col, index_row;
         /* Find the 1D index of block */
        block = index / (blocksize * blocksize);
        /* Find the 1D index of the element relative 
         to the block it’s in */
        within_index = index - (block * blocksize * blocksize); 
        /* Find 2D index relative to its block */
        within_col = within_index % blocksize; 
        within_row = within_index / blocksize;
        /* Find 2D index of block relative to others */
        block_num = (width + blocksize - 1)/blocksize;
        block_col = block % block_num; 
        block_row = block / block_num;
        /* Find final 2D index, and add it to the pixel struct to return */
        index_col = (block_col * blocksize) + within_col;
        index_row = (block_row * blocksize) + within_row; 
        location mylocation = {index_col, index_row};
        return mylocation;
}

/* Simply checks, given a [col, row] index and a width and height,
*  whether an index is valid or not (returns 1 if valid). */
int is_valid(int col, int row, int width, int height) 
{
        return (col < width && row < height && col >= 0 && row >= 0);
}

/* 
* Returns a pointer to the cell in the given column and row.
* If index is out of range, we have a checked run-time error.
*/
void *UArray2b_at(T array2b, int column, int row) 
{
        assert (is_valid (column, row, array2b -> width, array2b -> height));
        int index = convert_to_1D(column, row, array2b -> blocksize, 
                                                        array2b -> width); 
        return UArray_at(array2b -> blocks, index);
}

/* Mapping function that visits every cell in one block 
*  before moving to another block. It takes in an UArray2b,
*  an apply function, and a closure, allowing the client to apply
*  a function to every element in block-major order. Note that
*  because we have some wasted space in our representation
*  of a 1D array, we have to convert our indices to 2D and check
*  whether it's valid each time, causing performance lag. */
void UArray2b_map (T array2b,
void apply(int col, int row, T array2b, void *elem, void *cl),
void *cl)
{
        assert (array2b != NULL);
        for (int i = 0; i < UArray_length((array2b -> blocks)); i++)
        {
                location loc = convert_to_2D(i, array2b -> blocksize, 
                                                        array2b -> width);
                /* If an index is valid, apply the function 
                        to the element there */
                if (is_valid (loc.col, loc.row, 
                                array2b -> width, array2b -> height)) {
                        apply (loc.col, loc.row, array2b, 
                                        UArray_at(array2b -> blocks, i), cl);
                }
        }
}
/*
* It is a checked run-time error to pass a NULL T
* to any function in this interface
*/
#undef T






