#include "explore_and_unblack.h"


/* 
        This helper function makes an index struct out of a given
        col and row, and pushes it to the stack. 
        INPUT: A stack, two ints (row and col)
        OUTPUT: Void
*/
static void push_to_stack (Stack_T stack, int col, int row)
{
        index *index_p = malloc(sizeof(index));
        index_p -> col = col; 
        index_p -> row = row; 
        Stack_push (stack, index_p);
}

void traverse_edges(Bit2_T bitmap, Stack_T stack, Bit2_T visited)
{       
        int row = 0;
        int col = 0; 
        int width = Bit2_width (bitmap);
        int height = Bit2_height (bitmap);
        /* Traverse columns and add black edge pixels to stack */
        while (col < width) {
                if (Bit2_get(bitmap, col, 0) == 1) {
                        push_to_stack (stack, col, 0);
                        unblack (bitmap, stack, visited); 
                }
                if (Bit2_get(bitmap, col, height - 1) == 1)  {
                        push_to_stack (stack, col, height - 1);
                        unblack (bitmap, stack, visited);
                }
                col++;
        }   
        /* Traverse rows and add black edge pixels to stack */
        while (row < height) {
                if (Bit2_get(bitmap, 0, row) == 1) {
                        push_to_stack (stack, 0, row);
                        unblack(bitmap, stack, visited); 
                }
                if (Bit2_get(bitmap, width - 1, row) == 1)  {
                        push_to_stack (stack, width - 1, row);
                        unblack(bitmap, stack, visited);
                }
                row++;
        }
}

/* 
        Checks whether an index has a black, unvisited pixel.
        INPUT: Two Bit2_Ts (one for the picture, one for a visited check),
        two ints (col and row).
        OUTPUT: An int (only 1 or 0),
*/
static int unvisited_black (Bit2_T bitmap, Bit2_T visited, int col, int row)
{
        return ((col >= 0) && (col < Bit2_width (bitmap)) && 
                (row >= 0) && (row < Bit2_height(bitmap)) &&
                (Bit2_get (bitmap, col, row) == 1) &&
                (Bit2_get (visited, col, row) == 0));
}

void unblack (Bit2_T bitmap, Stack_T stack, Bit2_T visited)
{ 
        while (!Stack_empty (stack)) {
                index *popped;
                /* Initialize pixel to be unblacked */
                popped = (index *)Stack_pop(stack);
                int col = popped -> col;
                int row = popped -> row;
                if (Bit2_get(visited, col, row) == 0) {
                        /* Mark that we've visited this pixel, and push
                            unvisited black neighbors to stack */
                        Bit2_put(visited, col, row, 1); 
                        if (unvisited_black (bitmap, visited, col, row+1)) {
                                push_to_stack (stack, col, row + 1);
                         }
                        if (unvisited_black (bitmap, visited, col + 1, row)) {
                                push_to_stack (stack, col + 1, row);
                        }
                        if (unvisited_black (bitmap, visited, col, row-1)) {
                                push_to_stack (stack, col, row - 1);
                        }
                        if (unvisited_black (bitmap, visited, col - 1, row)) {
                                push_to_stack (stack, col - 1, row); 
                        }
                }
                /* Unblack and free the pixel */
                Bit2_put(bitmap, col, row, 0);
                free(popped);
        }
}
