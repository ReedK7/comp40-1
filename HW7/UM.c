/*
 * UM.c
 * Ethan Chen and Genevieve Silverman
 * HW 6 - UM
 *
 * UM.c contains the main function for our um program that behaves
 * like a 32 bit machine, storing data and performing 32 bit instructions
 * 
 * compile command: $ make um
 * execute command: $ ./um [filename]
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <except.h> 
#include <seq.h>
#include "Machine.h"

static const int N_ARGS = 2;
static const int N_REGISTER = 8;
static const int INIT_VAL = 0;

int main(int argc, char *argv[])
{
        int32_t nInstructions;
        void *instructions;
        UMachine_T um;

        if (argc != N_ARGS) {
                fprintf(stderr, "usage: ./um [filename]\n");
                exit(EXIT_FAILURE);
        }

        nInstructions = getInstructionCount(argv[1], sizeof(uint32_t));
        instructions = storeInstructions(argv[1], sizeof(uint32_t));
        um = Machine_new(N_REGISTER, nInstructions, 0);

        Machine_storeInstructions(um, instructions, nInstructions);
        Machine_runProgram(um);

        return 0;
}
