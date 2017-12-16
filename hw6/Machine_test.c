/*
 * Machine_test.c
 * Ethan Chen and Genevieve Silverman
 * HW 6 - UM
 *
 * Machine_test.c runs tests on the Machine.c implementation and
 * outputs the test results to stderr
 *
 * usage: ./Machine_test < allASCIIchars.txt > /dev/null
 */

#include <stdint.h>
#include <stdio.h>
#include <except.h>
#include "Machine.h"
#include "Machine_test.h"

static void machineNew_test(); 
static void MachineFreeNull_test();


static void printErrors(int numErrors, char *test); 

int main () 
{
        fprintf(stderr, "   **** machine tests ****\n");

        machineNew_test();
        MachineFreeNull_test();

        printErrors(test_cmov(), "cmov:\t\t\t");
        printErrors(test_add(), "add:\t\t\t");
        printErrors(test_mul(), "mul:\t\t\t");
        printErrors(test_divide(), "divide:\t\t\t");
        printErrors(test_nand(), "nand:\t\t\t");
        printErrors(test_loadval(), "loadval:\t\t");
        printErrors(test_map(), "map:\t\t\t");
        printErrors(test_unmap(), "unmap:\t\t\t");
        printErrors(test_sload(), "sload:\t\t\t");
        printErrors(test_sstore(), "sstore:\t\t\t");
        printErrors(test_loadp(), "loadp:\t\t\t");
        printErrors(test_in(), "in:\t\t\t");
        printErrors(test_out(), "out:\t\t\t");
}

/*
 * this test creates a new UMachine_T instance and then frees it
 * it does not count errors, and is only meant as a test to be run with
 * valgrind 
 */
static void machineNew_test() 
{
        UMachine_T um = Machine_new(2, 2, 2); 
        Machine_free(&um);
}

/*
 * tests that Machine_free() cannot take a NULL pointer
 */
static void MachineFreeNull_test()
{
        int numErrors = 0;
        void *NULL_ptr = NULL;

        TRY
                Machine_free(NULL);
                numErrors++;
        ELSE;
        END_TRY;

        TRY
                Machine_free(NULL_ptr);
                numErrors++;
        ELSE;
        END_TRY;

        printErrors(numErrors, "Free - NULL:\t\t");
}

/*
 * prints the results of a unit test on standard error output
 */
static void printErrors(int numErrors, char *test)
{
        if (numErrors == 0) {
                fprintf(stderr, "%sPASSED\n", test);
        } else if (numErrors == 1) {
                fprintf(stderr, "%sFAILED - %d error\n", test, numErrors);
        } else {
                fprintf(stderr, "%sFAILED - %d error\n", test, numErrors);
        }
}