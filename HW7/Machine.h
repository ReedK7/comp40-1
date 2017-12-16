/*
 * Machine.h
 * Ethan Chen and Genevieve Silverman
 * HW 6 - UM
 *
 * Machine.h an interface that emulates a 32-bit machine that can 
 * execute 32-bit instructions
 */

#ifndef MACHINE_INCLUDED
#define MACHINE_INCLUDED

#include <stdint.h>

typedef struct UMachine_T *UMachine_T;

/*
 * creates a new instance of a UMachine_T with nRegisters number of registers
 * and a segment 0 of size segment0size
 * no C.R.E.s
 */
UMachine_T Machine_new(int32_t nRegisters, int32_t segment0size, 
                       uint32_t initVal);

/*
 * frees all memory on the heap used by a UMachine_T instance
 * C.R.E. if um is NULL or points to NULL
 */
/* void Machine_free(UMachine_T *um); */

/*
 * stores the instructions for a machine program
 * C.R.E. if um is NULL
 */
void Machine_storeInstructions(UMachine_T um, void *instructions, 
        int32_t seg0size);

/*
 * takes in a 32 bit instruction and performs it on the machine
 * C.R.E.s depend on the instruction (tbd)
 */
/* void Machine_performInstruction(UMachine_T um, uint32_t instruction, 
                                int32_t opcodeWidth, int32_t registerWidth); */

/*
 * runs instructions from segment 0 in order until halt
 */
void Machine_runProgram(UMachine_T um);

/*
 * parses the instructions out of a file and returns them in a contiguous
 * sequence in memory
 * C.R.E. if the number of instructions isn't an even integer or if the
 * file cannot be opened
 */
void *storeInstructions(char *filename, int32_t instrSize);

/*
 * calculates the number of instructions in a file
 * C.R.E. if the number of instructions isn't an even integer or if the
 * file cannot be opened
 */
int32_t getInstructionCount(char *filename, int32_t instrSize);


#endif