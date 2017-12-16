/*
 * IO.c
 * Ethan Chen and Genevieve Silverman
 * HW 6 - UM
 *
 * IO.c has the implementation of the IO interface for
 * the IO device used in the UM
 */

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "IO.h"

/*
 * output()
 * purpose: outputs byte on standard output 
 * parameters: uint8_t (a byte to be ouptutted to standard output)
 * return type: void
 * errors thrown: assertion error when an error occurs in putchar() to stdout
 */
void output(uint8_t byte)
{
        int32_t returnSignal = putchar(byte);
        assert(returnSignal != EOF);
}

/* 
 * input()
 * purpose: returns a byte parsed from standard input
 * parameters: none
 * return type: uint32_t (the byte parsed from standard input)
 * errors thrown: assertion exception when an error is detected in stdin 
 *                by ferror()
 */
uint32_t input()
{
        uint32_t ch = getchar();
        assert(!ferror(stdin));

        return ch;
}