/*********************************************************
* Ethan Chen and Edwin Jain
* Homework 2
*
* This is the header file for print_format. This module simply
* formats our output after we've removed black edges from a pic.
**********************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <bit2.h>

/* Format the beginning of the output for a valid PBM from a Bit2_T */
void format_output (Bit2_T bitmap);

/* Prints all the bits using map_row_major in plain PBM format */
void print_bits (int col, int row, Bit2_T bitmap, int val, void *cl);

