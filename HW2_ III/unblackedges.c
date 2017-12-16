
#include "handlepic.h"
#include "print_format.h"
#include "explore_and_unblack.h"


int main (int argc, char* argv[])
{       
        FILE *fp; 
        Pnmrdr_T input;

        if (argc == 2) {
                fp = fopen(argv[1], "rd"); 
                input = check_valid (fp, argc);
        }
        else if (argc == 1) {
                input = check_valid (stdin, argc);
        }
        else {
                fprintf (stderr, "Too many args");
        } 
        Bit2_T bitmap = save_bits (input);
        Bit2_T visited = Bit2_new(Bit2_width(bitmap), Bit2_height(bitmap));
        Stack_T stack = Stack_new ();
        traverse_edges (bitmap, stack, visited);
        format_output (bitmap);
        Bit2_free(&bitmap);
        Bit2_free(&visited);
        Stack_free(&stack);
        close_and_free(fp, argc, input);
        return 0;
}



