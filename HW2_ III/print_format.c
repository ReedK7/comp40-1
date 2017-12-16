#include "print_format.h"


void format_output (Bit2_T bitmap)
{
        printf("P1\n");
        printf ("%d %d\n", Bit2_width(bitmap), Bit2_height(bitmap));
        int *counter = malloc(sizeof(int));
        *counter = 0; 
        Bit2_map_row_major (bitmap, print_bits, counter);
}

void print_bits (int col, int row, Bit2_T bitmap, int val, void *cl)
{
        (void) row;
        (*((int *)(cl)))++;
        /* If we get to the last column of the picture, or we get to 70
            columns, then we insert a newline into our output. */
        if (col == Bit2_width(bitmap) - 1 || (*((int *) (cl))) == 70) {
                printf ("%d\n", val);
                (*((int *)(cl))) = 0; 
        } else {
                printf ("%d", val);
        }
}



