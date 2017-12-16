/***************************************************************************
 * Ethan Chen and Edwin Jain 
 * Homework 2 
 * 
 * This program check whether a sudoku solution is valid or not. 
 * As we are reading in values from the file, we use 3 Bit2s to determine 
 * whether or not that value has been seen in it's respective row/column/box. 
 * 
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include "bit2.h"
#include <pnmrdr.h>
#include "except.h"
#include <assert.h>

const int SUDOKU_SIZE = 81;
const int SUDOKU_DIM = 9;

void close_and_free(FILE *fp, int argc, Pnmrdr_T pic); 
Pnmrdr_T check_valid_pnm (FILE *fp, int argc);
int check_row (Bit2_T row_array, int pixel, int row);
int check_col(Bit2_T col_array, int pixel, int col);
int check_box(Bit2_T box_array, int pixel, int col, int row);

Except_T Not_Valid_Pnm_E = {"Not a valid Pnm file"};
Except_T Zero_Pixel_E = {"Pixel with 0 intensity not allowed"};

/*
        This function creates 3 Bit2_T's to check whether a value has 
        been repeated in a row/column/box. When a number is read, 
        we go to the index [col, value] in the column Bit2 and put a 1 there. 
        We apply the same idea to the row Bit2 and the box Bit2 but the 
        indices marked in the Bit2 are [row, value] or [box, value]. If a 1 
        was there before then we already had a 1 in that column and the 
        solution is invalid. 
*/ 
int main (int argc, char *argv[])
{
        FILE *input;
        int retvalue = 0;
        int goodpixel = 1;
        if (argc == 1) {
                input = (stdin);
        }
        else if (argc == 2) {
                input = fopen(argv[1], "rd"); 
        }
        else {
                fprintf(stderr, "Provide only one file name\n");
                exit (EXIT_FAILURE);
        }
        Pnmrdr_T pic = check_valid_pnm (input, argc);
        /*Creates the bit2s*/ 
        Bit2_T column_array = Bit2_new (SUDOKU_DIM, SUDOKU_DIM);
        Bit2_T row_array = Bit2_new (SUDOKU_DIM, SUDOKU_DIM);
        Bit2_T box_array = Bit2_new (SUDOKU_DIM, SUDOKU_DIM);
        for (int row = 0; row < SUDOKU_DIM; row++) { 
                for (int col = 0; col < SUDOKU_DIM; col++)
                {
                        int pixel = (int)Pnmrdr_get (pic); 
                        goodpixel = goodpixel && (pixel > 0);
                        /*returns a 1 if a number has been repeated 
                           in a row/column/box and then lazy evaluation 
                           allows us to skip checking the rest of the values */
                        if (goodpixel) {
                                retvalue = 
                                (retvalue ||
                                (check_row (row_array, pixel, row)) ||
                                (check_col (column_array, pixel, col)) ||
                                (check_box (box_array, pixel, col, row)));
                        }
                }
        }
        Bit2_free(&row_array);
        Bit2_free(&column_array);
        Bit2_free(&box_array);
        close_and_free(input, argc, pic);
        if (!goodpixel) {
                RAISE (Zero_Pixel_E);         
        }
        return retvalue; 
}

/* Closes the pic and frees memory allocated for it */
void close_and_free(FILE *fp, int argc, Pnmrdr_T pic) 
{
         Pnmrdr_free(&pic);
         if (argc == 2) {
                 fclose(fp);
         }
}

/* 
        Checks that the file given is a valid pnm file
        INPUT: A file pointer and the number of arguments
        OUTPUT: Pnmrdr_T
*/  
Pnmrdr_T check_valid_pnm (FILE *fp, int argc)
{
        Pnmrdr_T pic = Pnmrdr_new (fp);
        Pnmrdr_mapdata data = Pnmrdr_data (pic);
        if ( (data.type != Pnmrdr_gray) ||
              ((int)data.width != SUDOKU_DIM) ||
              ((int)data.height != SUDOKU_DIM) ||
              ((int)data.denominator != SUDOKU_DIM)) {
                close_and_free(fp, argc, pic);
                RAISE (Not_Valid_Pnm_E); 
        }
        return pic;
}

/*
        Checks the relevant row, col, or box for the index corresponding
        to [row/col/box, pixel]. If we have a valid solution so far, then 
        these functions will return 0 to our conditional in main.  
        INPUT: A Bit2_T, an int for the pixel value, and ints for the index
        as needed (row, col for check_box).
        OUTPUT: An int (0 or 1)
*/ 
int check_row (Bit2_T row_array, int pixel, int row)
{ 
        return (Bit2_put (row_array, row, pixel-1, 1)); 
} 

int check_col(Bit2_T col_array, int pixel, int col) 
{
        return (Bit2_put(col_array, col, pixel -1, 1));
}

int check_box(Bit2_T box_array, int pixel, int col, int row) 
{ 
        /* We can determine the box of any index using the formula below.
            Note that this method numbers boxes in English reading order */
        int box = (col/3) + (3 * (row/3)); 
        return (Bit2_put(box_array, box, pixel-1, 1));
}





