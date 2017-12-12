/*
 *     readaline.c
 *     by Ethan Chen (echen11) and Genevieve Silverman (gsilve02), 9/15/17
 *     HW1 Part B - Readaline
 *
 *     Readaline is a function that can read a line from a file, allocate
 *     space to store that line, and return the line and the number
 *     of bytes in the line to its caller. Readaline is able to parse
 *     lines of greater than 200 bytes.
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "readaline.h"
#include "except.h"

#define INIT_NUM_BYTES 200

Except_T Null_Parameter_E = { "one or more readaline() parameters are NULL" };
Except_T File_E = { "error while reading a file" };
Except_T Malloc_E = { "error while allocating memory" };

void append_char(char **linepp, char ch, int *i);
void resize_string(char **linepp, int *bytes_allocated);

size_t readaline(FILE *inputfd, char **datapp)
{
        char *linep;
        char ch;
        int curr_num_chars = 0;
        int bytes_allocated = INIT_NUM_BYTES;

        /* error handling & key initializations */
        if (inputfd == NULL || datapp == NULL) {
                RAISE(Null_Parameter_E);
        }

        ch = fgetc(inputfd);
        if (feof(inputfd)) {
                *datapp = NULL;
                return 0;
        }

        linep = malloc(INIT_NUM_BYTES);
        if (linep == NULL) {
                RAISE(Malloc_E);
        }

        /* read file line char by char until EOF, error or \n */
        while (!feof(inputfd) && !ferror(inputfd)) {
                append_char(&linep, ch, &curr_num_chars);

                if (curr_num_chars >= bytes_allocated - 1) {
                        resize_string(&linep, &bytes_allocated);
                }
                if (ch == '\n') {
                        break;
                }
                ch = fgetc(inputfd);
        }

        if (ferror(inputfd)) {
                RAISE(File_E);
        }

        *datapp = linep;
        return curr_num_chars;
}

void append_char(char **linepp, char ch, int *i)
{
        *(*linepp + *i) = ch;
        *i += 1;
}

void resize_string(char **linepp, int *bytes_allocated)
{
        *bytes_allocated = (*bytes_allocated * 2) + 1;
        *linepp = realloc(*linepp, *bytes_allocated);
        if (*linepp == NULL) {
                RAISE(Malloc_E);
        }
}

#undef INIT_NUM_BYTES

