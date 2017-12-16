/*
 * Machine.c
 * Ethan Chen and Genevieve Silverman
 * HW 6 - UM
 *
 * Machine.c holds the implementation of an interface to emulate
 * a 32-bit machine that can execute 32-bit instructions
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <except.h> 
#include <seq.h>
#include "MemorySegments.h"
#include "Registers.h"
#include "Machine.h"
#include "bitpack.h"
#include "IO.h"

typedef uint32_t Um_register;

typedef enum Um_opcode {
        CMOV = 0, SLOAD = 1, SSTORE = 2, ADD = 3, MUL = 4, DIV = 5,
        NAND = 6, HALT = 7, MAP = 8, UNMAP = 9, OUTPUT = 10, INPUT = 11,
        LOADP = 12, LV = 13
} Um_opcode;

/* constants for UM functioning */
static const int32_t INSTRUCTION_SIZE = 32;
static const uint32_t LOADVAL_WIDTH = 25; 
static const uint32_t OPCODE_WIDTH = 4;
static const uint32_t REGISTER_WIDTH = 3;
static const uint32_t INIT_VAL = 0; 

/* constant for UM tests */
static const int TEST_REG_NUM = 8;


/* representation of the machine */
struct UMachine_T {
        MemorySegments_T memorySegments;
        Registers_T registers;
        uint32_t programCounter; /* holds the index in segment 0 of the 
                                  * next instruction */
};


/*****************************/
/*  instruction prototypes   */
/*****************************/

static void cmov   (Registers_T registers, Um_register ra, 
                                           Um_register rb, 
                                           Um_register rc);

static void sload  (MemorySegments_T memorySegments, Registers_T registers, 
                                           Um_register ra, 
                                           Um_register rb, 
                                           Um_register rc);

static void sstore (MemorySegments_T memorySegments, Registers_T registers, 
                                           Um_register ra,
                                           Um_register rb, 
                                           Um_register rc);

static void add    (Registers_T registers, Um_register ra, 
                                           Um_register rb, 
                                           Um_register rc);

static void mul    (Registers_T registers, Um_register ra, 
                                           Um_register rb, 
                                           Um_register rc);

static void divide (Registers_T registers, Um_register ra, 
                                           Um_register rb, 
                                           Um_register rc);

static void nand   (Registers_T registers, Um_register ra, 
                                           Um_register rb, 
                                           Um_register rc);

static void halt   (UMachine_T um);

static void map    (MemorySegments_T memorySegments, Registers_T registers, 
                                           Um_register rb, 
                                           Um_register rc);

static void unmap  (MemorySegments_T memorySegments, Registers_T registers, 
                                           Um_register rc);

static void out    (Registers_T registers, Um_register rc);

static void in     (Registers_T registers, Um_register rc);

static void loadp  (MemorySegments_T memorySegments, Registers_T registers,
                                           uint32_t *programCounter, 
                                           Um_register rb, 
                                           Um_register rc);

static void loadval(Registers_T registers, Um_register ra, unsigned val);




/************************************/
/*     function implementations     */
/************************************/

/*
 * cmov()
 * purpose: moves the value in register b into register a if register c's
 *          value is 0
 * parameters: Registers_T (registers representation), Um_register (register
 *          a), Um_register (register b), Um_register (register c) 
 * return type: void
 * errors thrown: none
 */
static void cmov   (Registers_T registers, Um_register ra, 
                                           Um_register rb, 
                                           Um_register rc)
{
        if (Registers_get(registers, rc) != 0) {
                Registers_put(registers, ra, Registers_get(registers, rb));
        }
}

/*
 * sload()
 * purpose: loads a value from a memory segment into a regiser 
 * parameters: MemorySegments_T (representation of memory segments), 
 *             Registers_T (representation of registers), Um_register 
 *             (register to put value in), Um_register (reigster holding
 *             memory segment id), Um_register (register holding index of
 *             value within memory segment)
 * return type: void
 * errors thrown: none
 */
static void sload  (MemorySegments_T memorySegments, Registers_T registers, 
                                           Um_register ra, 
                                           Um_register rb, 
                                           Um_register rc)
{
        uint32_t segmentId = Registers_get(registers, rb); 
        uint32_t i = Registers_get(registers, rc);
        uint32_t value; 
        
        if (segmentId == 0) {
                value = MemorySegments_getSegment0(memorySegments, i); 
        } else {
                value = MemorySegments_get(memorySegments, segmentId, i); 
        }
        Registers_put(registers, ra, value);
}

/*
 * sstore()
 * purpose: stores a value held in a register into a memory segment
 * parameters: MemorySegments_T (representation of memory segments), 
 *             Registers_T (representation of registers), Um_register 
 *             (register holding segment id), Um_register (reigster holding
 *             index of memory segment to put value in), Um_register (register 
 *             holding value to put in memory segment)
 * return type: void
 * errors thrown: none
 */
static void sstore (MemorySegments_T memorySegments, Registers_T registers, 
                                           Um_register ra,
                                           Um_register rb, 
                                           Um_register rc)
{
        uint32_t segmentId = Registers_get(registers, ra); 
        uint32_t i = Registers_get(registers, rb);
        uint32_t value = Registers_get(registers, rc);
        
        if (segmentId == 0) {
                MemorySegments_putSegment0(memorySegments, i, value);
        } else {
                MemorySegments_put(memorySegments, segmentId, i, value);
        }
}

/*
 * add()
 * purpose: adds the values in two registers and stores it in another register
 * parameters: Registers_T (registers representation), Um_register 
 *             (register to hold sum), Um_register (register holding
 *             one of the values in addition equation), Um_regsiter (register
 *             holding second value in addition equation)
 * return type: void
 * errors thrown: none
 */
static void add    (Registers_T registers, Um_register ra, 
                                           Um_register rb, 
                                           Um_register rc) 
{
        uint32_t rbVal, rcVal; 
        rbVal = Registers_get(registers, rb);
        rcVal = Registers_get(registers, rc);  

        Registers_put(registers, ra, (rbVal + rcVal));
}

/*
 * mul()
 * purpose: multiplies two values in registers and puts the product in another
 *          register
 * parameters: Registers_T (registers representation), Um_register 
 *             (register to hold product), Um_register (register holding
 *             one of the values in multiplication equation), Um_regsiter 
 *             (register holding second value in multiplication equation)
 * return type: void
 * errors thrown: none
 */
static void mul    (Registers_T registers, Um_register ra, 
                                           Um_register rb, 
                                           Um_register rc)
{
        uint32_t rbVal, rcVal; 
        rbVal = Registers_get(registers, rb);
        rcVal = Registers_get(registers, rc);

        Registers_put(registers, ra, (rbVal * rcVal));
}

/*
 * divide()
 * purpose: divides two values in registers and puts the result in another
 *          register
 * parameters: Registers_T (registers representation), Um_register 
 *             (register to hold result), Um_register (register holding
 *             one of the values in division equation), Um_regsiter 
 *             (register holding second value in division equation)
 * return type: void
 * errors thrown: none
 */
static void divide (Registers_T registers, Um_register ra, 
                                           Um_register rb, 
                                           Um_register rc)
{
        uint32_t rbVal, rcVal; 
        rbVal = Registers_get(registers, rb);
        rcVal = Registers_get(registers, rc);  

        Registers_put(registers, ra, (rbVal / rcVal));
}

/*
 * nand()
 * purpose: 'ands' the bits of two numbers held in registers together and
 *          takes the opposite bit representation of that bit sequence and 
 *          stores it in a register
 * parameters: Registers_T (registers representation), Um_register 
 *             (register to hold result), Um_register (register holding
 *             one of the values in the nand operation), Um_regsiter 
 *             (register holding second value in nand operation)
 * return type: void
 * errors thrown: none
 */
static void nand   (Registers_T registers, Um_register ra, 
                                           Um_register rb, 
                                           Um_register rc)
{
        uint32_t rbVal, rcVal; 
        rbVal = Registers_get(registers, rb);
        rcVal = Registers_get(registers, rc);

        Registers_put(registers, ra, ~(rbVal & rcVal));
}

/*
 * halt()
 * purpose: stops um execution and ends program
 * parameters: UMachine_T (machine to be stopped)
 * return type: void
 * errors thrown: none
 */
static void halt(UMachine_T um)
{
        Machine_free(&um);
        exit(0);
}

/*
 * map()
 * purpose: creates a memory segment with a specific number of words
 * parameters: MemorySegments_T (memory segments representation), Regisers_T
 *             (register representation), Um_regsiter (register to store
 *             new memory segment's id in), Um_register (register holding
 *             the number of words that the new memory segment should have)
 * return type: void
 * errors thrown: none
 */
static void map    (MemorySegments_T memorySegments, Registers_T registers, 
                                           Um_register rb, 
                                           Um_register rc)
{
        uint32_t nWords;
        uint32_t segmentId; 

        nWords = Registers_get(registers, rc);
        segmentId = MemorySegments_map(memorySegments, nWords, INIT_VAL); 

        Registers_put(registers, rb, segmentId);
}

/*
 * unmap()
 * purpose: removes a memory segment
 * parameters: MemorySegments_T (memory segments representation), Regisers_T
 *             (register representation), Um_regsiter (register holding
 *             the id of the memory segment to remove)
 * return type: void
 * errors thrown: none
 */
static void unmap  (MemorySegments_T memorySegments, Registers_T registers, 
                                           Um_register rc)
{
        uint32_t segmentId = Registers_get(registers, rc);
        MemorySegments_unmap(memorySegments, segmentId); 
}

/* 
 * out()
 * purpose: to output a byte from standard output
 * parametesr: Registers_T (registers representation), Um_register (register
 *             holding byte to output)
 * return type: void
 * errors thrown: none
 */
static void out    (Registers_T registers, Um_register rc)
{
        uint8_t ch = Registers_get(registers, rc); 
        output(ch);
}

/* 
 * in()
 * purpose: to input a byte from standard input
 * parametesr: Registers_T (registers representation), Um_register (register
 *             to hold byte inputted)
 * return type: void
 * errors thrown: none
 */
static void in     (Registers_T registers, Um_register rc)
{
        uint32_t ch = input();
        Registers_put(registers, rc, ch); 
        
}

/*
 * loadp()
 * purpose: duplicates a memory segment and stores it in segment 0 (containing
 *          the program instructions), removing segment 0, and resetting
 *          the program counter
 * parameters: MemorySegments_T (memory segments representation), Registers_T
 *             (registers representation), uint32_t *(pointer to the 
 *             program counter), Um_register (register storing the memory
 *             segment id of the memory segment to be duplicated and set as
 *             segment 0), Um_register (register storing the new program
 *             counter value)
 * return type: void
 * errors thrown: none
 */ 
static void loadp  (MemorySegments_T memorySegments, Registers_T registers,
                                           uint32_t *programCounter, 
                                           Um_register rb, 
                                           Um_register rc)
{ 
        uint32_t segmentId = Registers_get(registers, rb); 

        if (segmentId != 0) {
                MemorySegments_switchSegment0(memorySegments, segmentId); 
        }

        *programCounter = Registers_get(registers, rc);
}

/*
 * loadval()
 * purpose: loads a sequence of bytes into a register
 * parameters: Registers_T (registers representation), Um_register (register
 *             to store value in), unsigned (bit sequence to store in
 *             register)
 * return type: void
 * errors thrown: none
 */
static void loadval(Registers_T registers, Um_register ra, unsigned val) 
{ 
        Registers_put(registers, ra, val);
}


/*
 * Machine_new()
 * purpose: creates a new instance of a UMachine_T with nRegisters number of
 *          registers and a segment 0 of size segment0size
 * parameters: int32_t (number of registers for machine to have), int32_t 
 *             (capacity of segment 0), uint32_t (value to initialize
 *              registers with)
 * return type: UMachine_T (machine representation)
 * errors thrown: assertion exception if malloc fails
 */
UMachine_T Machine_new(int32_t nRegisters, int32_t segment0size, 
                       uint32_t initVal)
{   
        UMachine_T Um = malloc(sizeof(*Um)); 
      
        assert(Um != NULL);
        
        Um -> registers = Registers_new(nRegisters, initVal);
        Um -> memorySegments = MemorySegments_new(segment0size);       
        Um -> programCounter = 0; 
           
        return Um;        
}

/*
 * Machine_free()
 * purpose: frees all memory on the heap used by a UMachine_T instance
 * parameters: UMachine_T *(pointer to a machine representation to free)
 * return type: void
 * errors thrown: assertion exception if um is NULL or points to NULL
 */
void Machine_free(UMachine_T *um)
{
        assert(um != NULL && *um != NULL);
        MemorySegments_free(&((*um) -> memorySegments));
        Registers_free(&((*um) -> registers));
        free(*um);
}

/*
 * Machine_storeInstructions()
 * purpose: stores the instructions for a machine program
 * parameters: UMachine_T (machine representation), void *(pointer to
 *             sequence of instructinos)
 * return type: voide
 * errors thrown: assertion exception if um is NULL 
 */
void Machine_storeInstructions(UMachine_T um, void *instructions)
{
        assert(um != NULL);
        MemorySegments_T memorySegments = um -> memorySegments; 
        MemorySegments_storeSegment0(memorySegments, instructions); 
}

/*
 * Machine_performInstruction()
 * purpose: takes in a 32 bit instruction and performs it on the machine
 * parameters: UMachine_T (machine representation), uint32_t (32 bit
 *             instruction), int32_t (width of the opcode in the instruction),
 *             int32_t (width of the register identifier in the instruction)
 * return type: void
 * errors thrown: assertion exception if um is NULL, exit(1) if opcode 
 *                unrecognized
 */
void Machine_performInstruction(UMachine_T um, uint32_t instruction, 
                                int32_t opcodeWidth, int32_t registerWidth)
{
        int32_t opcode, ra, rb, rc;
        uint32_t value; 
        Registers_T registers;

        assert (um != NULL); 

        registers = um -> registers; 
        opcode = Bitpack_getu(instruction, opcodeWidth, 
                             (INSTRUCTION_SIZE - opcodeWidth)); 

        ra = Bitpack_getu(instruction, registerWidth, (registerWidth * 2));
        rb = Bitpack_getu(instruction, registerWidth, registerWidth);
        rc = Bitpack_getu(instruction, registerWidth, 0);
                 
        if (opcode ==  CMOV) {
                cmov(registers, ra, rb, rc);
                (um -> programCounter)++;
        } else if (opcode == SLOAD) {
                sload(um -> memorySegments, registers, ra, rb, rc);
                (um -> programCounter)++;
        } else if (opcode == SSTORE) {
                sstore(um -> memorySegments, registers, ra, rb, rc);
                (um -> programCounter)++;
        } else if (opcode == ADD) {
                add(registers, ra, rb, rc);
                (um -> programCounter)++;
        } else if (opcode == MUL) {
                mul(registers, ra, rb, rc);
                (um -> programCounter)++;
        } else if (opcode == DIV) {
                divide(registers, ra, rb, rc);
                (um -> programCounter)++;
        } else if (opcode == NAND) {
                nand(registers, ra, rb, rc);
                (um -> programCounter)++;
        } else if (opcode == HALT) {
                halt(um);
        } else if (opcode == MAP) {
                map(um -> memorySegments, registers, rb, rc);
                (um -> programCounter)++;
        } else if (opcode == UNMAP) {
                unmap(um -> memorySegments, registers, rc); 
                (um -> programCounter)++;
        } else if (opcode == OUTPUT) {
                out(registers, rc); 
                (um -> programCounter)++;
        } else if (opcode == INPUT) {
                in(registers, rc); 
                (um -> programCounter)++;
        } else if (opcode == LOADP) {
                loadp(um -> memorySegments, registers, &(um -> programCounter), 
                      rb, rc); 
        } else if (opcode == LV) {
                ra = Bitpack_getu(instruction, registerWidth, 
                              INSTRUCTION_SIZE - registerWidth - opcodeWidth);
                value = Bitpack_getu(instruction, LOADVAL_WIDTH, 0); 
                loadval(registers, ra, value);
                (um -> programCounter)++;
        } else { 
                exit(1); 
        }
}

/*
 * Machine_runProgram()
 * purpose: runs a program with the machine
 * parameters: UMachine_T (machine representation)
 * return type: void
 * errors thrown: assertion exception if um is null
 */
void Machine_runProgram(UMachine_T um)
{
        uint32_t instruction;

        assert(um != NULL);
        while (true) {
                instruction = MemorySegments_getSegment0(um -> memorySegments, 
                                                         um -> programCounter);
                Machine_performInstruction(um, instruction, OPCODE_WIDTH, 
                                           REGISTER_WIDTH);
        }
}


/************************************/
/*          test functions          */
/************************************/

/*
 * tests cmov and returns the number of errors found
 */
int test_cmov()
{
        int numErrors = 0;
        UMachine_T um = Machine_new(TEST_REG_NUM, TEST_REG_NUM, 1);
        Registers_T registers = um -> registers;

        uint32_t ra = 0, raVal = 10;
        uint32_t rb = 1, rbVal = raVal * rb + raVal; 
        uint32_t rc = 2, rcVal = raVal * rc + raVal;

        Registers_put(registers, ra, raVal);
        Registers_put(registers, rb, rbVal);
        Registers_put(registers, rc, rcVal);

        cmov(registers, ra, rb, rc);

        if (Registers_get(registers, ra) != rbVal) {
                numErrors++;
        }

        Registers_put(registers, ra, raVal);
        Registers_put(registers, rc, 0);

        if (Registers_get(registers, ra) != raVal) {
                numErrors++;
        }

        Machine_free(&um);
        return numErrors;
}

/*
 * tests sload and returns the number of errors found
 */
int test_sload()
{
        int numErrors = 0;
        uint32_t val = ~0, i = 1;
        uint32_t ra = 0, rb = 1, rc = 2;

        UMachine_T um = Machine_new(TEST_REG_NUM, TEST_REG_NUM, 1);
        Registers_T registers = um -> registers;
        MemorySegments_T memorySegments = um -> memorySegments;

        uint32_t segmentId = MemorySegments_map(memorySegments, 10, INIT_VAL);
        MemorySegments_put(memorySegments, segmentId, i, val);

        Registers_put(registers, rb, segmentId);
        Registers_put(registers, rc, i);

        sload(memorySegments, registers, ra, rb, rc);

        if (Registers_get(registers, ra) != val) {
                numErrors++;
        }

        Machine_free(&um);
        return numErrors;
}

/*
 * tests sstore and returns the number of errors found
 */
int test_sstore()
{
        int numErrors = 0;
        uint32_t val = ~0, i = 1;
        uint32_t ra = 0, rb = 1, rc = 2;

        UMachine_T um = Machine_new(TEST_REG_NUM, TEST_REG_NUM, 1);
        Registers_T registers = um -> registers;
        MemorySegments_T memorySegments = um -> memorySegments;

        uint32_t segmentId = MemorySegments_map(memorySegments, 10, INIT_VAL);
        
        Registers_put(registers, rc, val); 
        Registers_put(registers, ra, segmentId); 
        Registers_put(registers, rb, i); 

        sstore(memorySegments, registers, ra, rb, rc);

        if (MemorySegments_get(memorySegments, segmentId, i) != val) {
                numErrors++;
        }

        Machine_free(&um);
        return numErrors;
}

/*
 * tests add and returns the number of errors found
 */
int test_add()
{
        int numErrors = 0;
        UMachine_T um = Machine_new(TEST_REG_NUM, TEST_REG_NUM, 1);
        Registers_T registers = um -> registers;

        uint32_t ra = 0, raVal = 10;
        uint32_t rb = 1, rbVal = raVal * rb + raVal; 
        uint32_t rc = 2, rcVal = raVal * rc + raVal;

        Registers_put(registers, rb, rbVal);
        Registers_put(registers, rc, rcVal);

        add(registers, ra, rb, rc); 

        if (Registers_get(registers, ra) != (rbVal + rcVal)) {
                numErrors++;
        }

        Machine_free(&um); 
        return numErrors;

}

/*
 * tests mul and returns the number of errors found
 */
int test_mul()
{
        int numErrors = 0;
        UMachine_T um = Machine_new(TEST_REG_NUM, TEST_REG_NUM, 1);
        Registers_T registers = um -> registers;

        uint32_t ra = 0, raVal = 10;
        uint32_t rb = 1, rbVal = raVal * rb + raVal; 
        uint32_t rc = 2, rcVal = raVal * rc + raVal;

        Registers_put(registers, rb, rbVal);
        Registers_put(registers, rc, rcVal);

        mul(registers, ra, rb, rc); 

        if (Registers_get(registers, ra) != (rbVal * rcVal)) {
                numErrors++;
        }

        Machine_free(&um); 
        return numErrors;
}

/*
 * tests divide and returns the number of errors found
 */
int test_divide()
{
        int numErrors = 0;
        UMachine_T um = Machine_new(TEST_REG_NUM, TEST_REG_NUM, 1);
        Registers_T registers = um -> registers;

        uint32_t ra = 0, raVal = 10;
        uint32_t rb = 1, rbVal = raVal * rb + raVal; 
        uint32_t rc = 2, rcVal = raVal * rc + raVal;

        Registers_put(registers, rb, rbVal);
        Registers_put(registers, rc, rcVal);

        divide(registers, ra, rb, rc); 

        if (Registers_get(registers, ra) != (rbVal / rcVal)) {
                numErrors++;
        }

        Machine_free(&um); 
        return numErrors;
}

/*
 * tests nand and returns the number of errors found
 */
int test_nand()
{
        int numErrors = 0;
        UMachine_T um = Machine_new(TEST_REG_NUM, TEST_REG_NUM, 1);
        Registers_T registers = um -> registers;

        uint32_t ra = 0, raVal = 10;
        uint32_t rb = 1, rbVal = raVal * rb + raVal; 
        uint32_t rc = 2, rcVal = raVal * rc + raVal;

        Registers_put(registers, rb, rbVal);
        Registers_put(registers, rc, rcVal);

        nand(registers, ra, rb, rc); 

        if (Registers_get(registers, ra) != ~(rbVal & rcVal)) {
                numErrors++;
        }

        Machine_free(&um); 
        return numErrors;
}

/*
 * tests map and returns the number of errors found
 */
int test_map()
{
        int numErrors = 0;
        uint32_t rb = 1, rc = 2, rcVal = 10; 
        uint32_t segmentId, i; 

        UMachine_T um = Machine_new(TEST_REG_NUM, TEST_REG_NUM, 1);
        Registers_T registers = um -> registers;
        MemorySegments_T memorySegments = um -> memorySegments; 

        Registers_put(registers, rc, rcVal); 
        map(memorySegments, registers, rb, rc);

        segmentId = Registers_get(registers, rb);
        for (i = 0; i < rcVal; i++) {
                if (MemorySegments_get(memorySegments, segmentId, i) != 
                                                                INIT_VAL) {
                        numErrors++; 
                } 
        }
        Machine_free(&um);
        return numErrors; 

}

/*
 * tests unmap and returns the number of errors found
 */
int test_unmap()
{
        uint32_t rb = 1, rc = 2, rcVal = 10; 
        uint32_t segmentId; 
        int numErrors;

        UMachine_T um = Machine_new(TEST_REG_NUM, TEST_REG_NUM, 1);
        Registers_T registers = um -> registers;
        MemorySegments_T memorySegments = um -> memorySegments; 

        Registers_put(registers, rc, rcVal); 
        map(memorySegments, registers, rb, rc);

        segmentId = Registers_get(registers, rb);
        Registers_put(registers, rc, segmentId); 
        unmap(memorySegments, registers, rc);

        numErrors = 0;

        TRY
                MemorySegments_get(memorySegments, segmentId, 0);
                numErrors++; 
        ELSE; 
        END_TRY; 

        Machine_free(&um);
        return numErrors; 

}

/*
 * tests out and returns the number of errors found
 */
int test_out()
{
        int numErrors = 0;
        uint16_t i, max = 1;
        max = max << TEST_REG_NUM;

        for (i = 0; i < max; i++) {
                TRY
                        output(i);
                ELSE
                        numErrors++;
                END_TRY;
        }

        return numErrors; 
}

/*
 * tests in and returns the number of errors found
 */
int test_in()
{
        int numErrors = 0;
        uint16_t i, max = 1, ch;
        max = max << TEST_REG_NUM;

        for (i = 0; i < max; i++) {
                TRY
                        ch = input(i);
                        assert(ch == (uint8_t)i);
                ELSE
                        numErrors++;
                END_TRY;
        }

        return numErrors; 
}

/*
 * tests loadp and returns the number of errors found
 */
int test_loadp()
{
        int numErrors = 0, i, segmentSize = 10;
        uint32_t rb = 0, rc = 1;
        UMachine_T um = Machine_new(TEST_REG_NUM, TEST_REG_NUM, 1);
        Registers_T registers = um -> registers;
        MemorySegments_T memorySegments = um -> memorySegments;

        uint32_t segmentId = MemorySegments_map(memorySegments, segmentSize, 
                                                INIT_VAL);

        for (i = 0; i < segmentSize; i++) {
                MemorySegments_put(memorySegments, segmentId, i, 
                                  (uint32_t)(~0 - i));
        }

        um -> programCounter = segmentSize;

        Registers_put(registers, rb, segmentId);
        Registers_put(registers, rc, 0);

        loadp(memorySegments, registers, &(um -> programCounter), rb, rc);

        if (Registers_get(registers, rc) != 0) {
                numErrors++;
        }

        for (i = 0; i < segmentSize; i++) {
                uint32_t instr = MemorySegments_getSegment0(memorySegments, i);
                if (instr != (uint32_t)(~0 - i)) {
                        numErrors++;
                }
        }

        Machine_free(&um);
        return numErrors;
}

/*
 * tests loadval and returns the number of errors found
 */
int test_loadval()
{
        int numErrors = 0;
        UMachine_T um = Machine_new(TEST_REG_NUM, TEST_REG_NUM, 1);
        Registers_T registers = um -> registers;
        uint32_t value = 100;
        uint32_t ra = 0; 
        loadval(registers, ra, value);

        if (Registers_get(registers, ra) != value) {
                numErrors++;
        }

        Machine_free(&um);
        return numErrors;
}

