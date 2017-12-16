/*
 * IO_test.c
 * Ethan Chen and Genevieve Silverman
 * HW 6 - UM
 *
 * IO_test.c contains unit test functions for the IO interface (IO.h)
 * the status of the tests are printed to stderr and the test output
 * of the tests are printed to stdout.
 *
 * compile command: $ make IO_test
 * execute command: ./IO_test < allASCIIchars.txt > /dev/null
 */

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "IO.h"

const int N_BITS = 8;

/* tests */
static void testOutputAllBytes();
static void testInputAllBytes();


int main()
{
        fprintf(stderr, "**********************************\n");

        fprintf(stderr, "testing output all bytes\n");
        testOutputAllBytes();
        fprintf(stderr, "output all bytes\n");

        fprintf(stderr, "**********************************\n");

        fprintf(stderr, "testing input all bytes\n");
        testInputAllBytes();
        fprintf(stderr, "input all bytes\n");

        fprintf(stderr, "**********************************\n");
        fprintf(stderr, "done!\n");

        return 0;
}

/*
 * tests the output function by calling output() on all ascii chars 0-255
 * test should be used by piping standard output to null stream
 */
static void testOutputAllBytes()
{
        uint16_t i, max = 1;
        max = max << N_BITS;

        for (i = 0; i < max; i++) {
                output(i);
        }
}

/*
 * this test is ment to work only to the specification in the file header
 * where allASCIIchars.txt is piped to standard intput. This test will then
 * read those characters in order from standard input and return them, 
 * checking that the expected character was retrieved 
 */
static void testInputAllBytes()
{
        uint16_t i, max = 1, ch;

        max = max << N_BITS;

        for (i = 0; i < max; i++) {
                ch = input(i);
                assert(ch == (uint8_t)i);
        }
}
