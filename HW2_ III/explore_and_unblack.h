/*********************************************************
* Ethan Chen and Edwin Jain
* Homework 2
*
* This is the header file for explore_and_unblack. This module 
* provides the functions that unblackedges uses to remove black
* edges. We use the stack provided by Hanson and a DFS algorithm
* to remove black pixels.
**********************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <bit2.h>
#include <stack.h>

/* We store the index as a struct of two ints (col and row). */

typedef struct index
{
    int col;
    int row;
} index;

/* 
        This traverses the edges of our Bit2_T (which contains the saved
        bits from our picture), pushes black edge pixels to the stack, and
        calls unblack.
        INPUT: A Bit2_T for the picture, a stack for black pixels, and a
        Bit2_T to check whether we've visited certain pixels.
        OUTPUT: Void
*/
void traverse_edges(Bit2_T bitmap, Stack_T stack, Bit2_T visited);

/*
        While the stack is not empty, this function pops off pixels from
        the stack, whitens them in the bitmap, and pushes all of the 
        pixel's unvisited black neighbors on the stack.
        INPUT; A Bit2_T for the picture, a stack for black pixels, and a
        Bit2_T to check whether we've visited certain pixels.
        OUTPUT: Void
*/  
void unblack (Bit2_T bitmap, Stack_T stack, Bit2_T visited);

