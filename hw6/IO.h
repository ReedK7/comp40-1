/*
 * IO.h
 * Ethan Chen and Genevieve Silverman
 * HW 6 - UM
 *
 * IO.h has the interface of the IO device for the UM
 */

#ifndef IO_INCLUDED
#define IO_INCLUDED

#include <stdint.h>

/*
 * outputs val on standard output 
 * C.R.E. if there's an error outputting to stdout
 */
void output(uint8_t byte);

/* 
 * returns a value parsed from standard input
 * C.R.E. if ferror(stdin) returns true
 */
uint32_t input();

#endif