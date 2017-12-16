/*********************************************************
* Ethan Chen and Edwin Jain
* Homework 2
*
* This is the header file for handlepic. This module simply
* handles reading in and writing pictures to a Bit2. We
* use this interface for our unblackedges program. 
**********************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <except.h>
#include <assert.h>
#include <bit2.h>
#include <pnmrdr.h>

/* 
        Check_valid checks that the file we're given (or stdin)
        contains a valid PNM and raises an error if not.
        INPUT: A file pointer and the number of arguments to main
        OUTPUT: A Pnmrdr_T picture, if it's a valid bitmap
*/
Pnmrdr_T check_valid(FILE *fp, int argc);

/*
        This gets called whenever we want to close a picture
        and free the memory allocated for it.
        INPUT: A file pointer, number of arguments, and a Pnmrdr_T
        OUTPUT: Void
*/
void close_and_free(FILE *fp, int argc, Pnmrdr_T pic);

/* 
        Creates a new Bit2_T that contains the bits from our picture.
        INPUT: A Pnmrdr_T pic
        OUTPUT: A Bit2_T 
*/
Bit2_T save_bits (Pnmrdr_T pic);
