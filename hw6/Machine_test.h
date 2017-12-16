/*
 * Machine_test.h
 * Ethan Chen and Genevieve Silverman
 * HW 6 - UM
 *
 * Machine_test.h contains an interface to test each of the instructions in 
 * the Machine implementation these tests can be run with Machine_test.c
 *
 * usage: ./Machine_test < allASCIIchars.txt > /dev/null
 */

#ifndef MACHINE_TEST_INCLUDED
#define MACHINE_TEST_INCLUDED

#include <stdint.h>
#include "Machine.h"

int test_cmov();
int test_sload();
int test_sstore();
int test_add();
int test_mul();
int test_divide();
int test_nand();
int test_map();
int test_unmap();
int test_out();
int test_in();
int test_loadp();
int test_loadval();


#endif