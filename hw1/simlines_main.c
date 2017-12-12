/*
 *     simlines_main.c
 *     by Ethan Chen (echen11) and Genevieve Silverman (gsilve02), 9/15/17
 *     HW1 Part C - Simlines
 *
 *     A file to run the simlines program that takes filenames on the 
 *     command line.
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include "simlines.h"

int main(int argc, char *argv[])
{
        if (argc > 1) {
                simlines(argc, argv);
        }

        return EXIT_SUCCESS;
}