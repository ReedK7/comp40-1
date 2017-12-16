/*
 * Machine.c
 * Ethan Chen and Genevieve Silverman
 * HW 7 - Profiling
 *
 * Machine.c holds the whole, optimized implementation of the um
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <except.h> 
#include <inttypes.h>
#include <seq.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "uarray.h"
#include "Machine.h"

/* conversions between segment index and segment id */
#define getSegmentId(segmentIndex) segmentIndex + 1
#define getSegmentIndex(segmentId) segmentId - 1

typedef uint32_t Um_register;
typedef struct MemorySegments_T *MemorySegments_T;
typedef struct Registers_T *Registers_T;

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

static const char *READ_MODE = "rd";
static const unsigned BITS_IN_BYTE = 8;
static const int DEFAULT_HINT = 500; 


/* representation of the machine */
struct UMachine_T {
        MemorySegments_T memorySegments;
        Registers_T registers;
        uint32_t programCounter; /* holds the index in segment 0 of the 
                                  * next instruction */
};

/* data abstraction for memory segments */
struct MemorySegments_T {
        uint32_t *segment0;
        uint32_t *segmentSizes;
        int32_t segmentCapacity;
        uint32_t **segments;
        int32_t numSegments; 
        int32_t *unmappedIndices;
        int32_t unmappedCapacity; 
        int32_t unmappedSize; 
};

struct Registers_T {
        int32_t size;
        uint32_t *registerArray;
};


/*****************************/
/*   static machine funcs    */
/*****************************/

/*
 * takes in a 32 bit instruction and performs it on the machine
 * C.R.E.s depend on the instruction (tbd)
 */
static inline void Machine_performInstruction(UMachine_T um, 
                                              uint32_t instruction, 
                                              int32_t opcodeWidth, 
                                              int32_t registerWidth);

/*
 * frees all memory on the heap used by a UMachine_T instance
 * C.R.E. if um is NULL or points to NULL
 */
static inline void Machine_free(UMachine_T *um);

static inline void parseInstructions(FILE *infile, uint32_t *instructions, 
                              int32_t instrSize, int32_t nInstructions);

static inline uint64_t shl(uint64_t word, unsigned bits);
static inline uint64_t shr(uint64_t word, unsigned bits);



/*****************************/
/*             io            */
/*****************************/

/*
 * outputs val on standard output 
 * C.R.E. if there's an error outputting to stdout
 */
static inline void output(uint8_t byte);

/* 
 * returns a value parsed from standard input
 * C.R.E. if ferror(stdin) returns true
 */
static inline uint32_t input();



/*****************************/
/*    mem segment funcs      */
/*****************************/

/*
 * creates a new instance of a MemorySegments_T
 * no C.R.E.s
 */
static inline MemorySegments_T MemorySegments_new(int32_t segment0length);

/*
 * frees any memory on the heap used by a MemorySegments_T instance
 * C.R.E. if memorySegments is NULL or points to NULL
 */
static inline void MemorySegments_free(MemorySegments_T *memorySegments);

/*
 * creates a new segment holding nWords number of words, each word 
 * initialized to initVal. The new segment is stored in memorySegments and
 * its id is returned from the function
 * C.R.E. if memorySegments is NULL
 */
static inline int32_t MemorySegments_map(MemorySegments_T memorySegments, 
                                         int32_t nWords, 
                                         uint32_t initVal);

/*
 * unmaps a segment with segmentId in memorySegments
 * C.R.E.s if memorySegments is NULL or if segmentId is unmapped already
 */
static inline void MemorySegments_unmap(MemorySegments_T memorySegments, 
                                        int32_t segmentId);

/*
 * puts a value in memorySegments's segment with segmentId at segmentIndex
 * C.R.E. if memorySegments is NULL or if segmentId is unmapped
 */
static inline void MemorySegments_put(MemorySegments_T memorySegments, 
                                      int32_t segmentId, int32_t i, 
                                      uint32_t val);

/*
 * returns a value in memorySegment's segment with segmentId at segmentIndex
 * C.R.E. if memorySegments is NULL or if segmentId is unmapped
 */
static inline uint32_t MemorySegments_get(MemorySegments_T memorySegments, 
                                          int32_t segmentId, int32_t i);

/*
 * puts instructions into segment 0 of memorySegments
 * C.R.E. if memorySegments is NULL
 */
static inline void MemorySegments_storeSegment0(MemorySegments_T memorySegments,
                                                void *instructions, 
                                                int32_t seg0size);

/*
 * returns the value stored in segment 0 at a certain index within
 * the segment
 * C.R.E. if memorySegments is NULL or if segmentIndex is out of bounds
 */
static inline uint32_t MemorySegments_getSegment0(
                                                MemorySegments_T memorySegments,
                                                  int32_t i);

/*
 * puts a value into segment 0
 * C.R.E. if memorySegments is NULL
 */
static inline void MemorySegments_putSegment0(MemorySegments_T memorySegments, 
                                              int32_t i, uint32_t val);

/*
 * puts a value into segment 0
 * C.R.E. replaces segment 0 with a copy of another segment
 */
static inline void MemorySegments_switchSegment0(
                                                MemorySegments_T memorySegments,
                                                 uint32_t segmentId); 

/* helper functions for MemorySegments_free() */
static inline void     free_segments       (uint32_t **segments, 
                                            int32_t capacity);

/* helper functions for MemorySegment_map() */
static inline uint32_t *new_segment         (int32_t nWords, uint32_t initVal);
static inline int32_t  reuseSegmentId       (int32_t *unmappedIndices, uint32_t 
                                             ***segments, uint32_t *newSegment, 
                                             int32_t *unmappedSize);
static inline int32_t  newSegmentId         (uint32_t ***segments, uint32_t 
                                             *newSegment, int32_t numSegments, 
                                             int32_t segmentCapacity, 
                                             bool *expanded);

/* helper functions for MemorySegment_unmap() */
static inline void     saveUnmappedIndex   (int32_t **unmappedIndices, 
                                            int32_t segmentIndex, 
                                            int32_t *unmappedSize, 
                                            int32_t *unmappedCapacity);


/*****************************/
/*     register funcs       */
/*****************************/

/* 
 * initializes an instance of Registers_T with nRegisters registers and 
 * each register holding a copy of initVal
 * no C.R.E.s
 */
static inline Registers_T Registers_new(int32_t nRegisters, uint32_t initVal);

/* 
 * destructs an instance of a Registers_T, freeing all of its memory
 * on the heap
 * C.R.E. if registers is NULL or points to NULL 
 */
static inline void Registers_free(Registers_T *registers);

/* 
 * retrieves the memory stored at register i 
 * C.R.E. if registers is NULL or if i is outside of scope
 */
static inline uint32_t Registers_get(Registers_T registers, int32_t i);

/* 
 * stores a value in register i of the Registers_T instance 
 * C.R.E. if registers is NULL or i is outside of scope
 */
static inline void Registers_put(Registers_T registers, int32_t i, 
                                 uint32_t val);


/*****************************/
/*  instruction prototypes   */
/*****************************/

static inline void cmov   (Registers_T registers, Um_register ra, 
                           Um_register rb, Um_register rc);

static inline void sload  (MemorySegments_T memorySegments, 
                           Registers_T registers, Um_register ra, 
                           Um_register rb, Um_register rc);

static inline void sstore (MemorySegments_T memorySegments, 
                           Registers_T registers, Um_register ra,
                           Um_register rb, Um_register rc);

static inline void add    (Registers_T registers, Um_register ra, 
                           Um_register rb, Um_register rc);

static inline void mul    (Registers_T registers, Um_register ra, 
                           Um_register rb, Um_register rc);

static inline void divide (Registers_T registers, Um_register ra, 
                           Um_register rb, Um_register rc);

static inline void nand   (Registers_T registers, Um_register ra, 
                           Um_register rb, Um_register rc);

static inline void halt   (UMachine_T um);

static inline void map    (MemorySegments_T memorySegments, 
                           Registers_T registers, Um_register rb, 
                           Um_register rc);

static inline void unmap  (MemorySegments_T memorySegments, 
                           Registers_T registers, Um_register rc);

static inline void out    (Registers_T registers, Um_register rc);

static inline void in     (Registers_T registers, Um_register rc);

static inline void loadp  (MemorySegments_T memorySegments, 
                           Registers_T registers, uint32_t *programCounter, 
                           Um_register rb, Um_register rc);

static inline void loadval(Registers_T registers, Um_register ra, unsigned val);




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
static inline void cmov   (Registers_T registers, Um_register ra, 
                           Um_register rb, Um_register rc)
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
static inline void sload  (MemorySegments_T memorySegments, 
                           Registers_T registers, Um_register ra, 
                           Um_register rb, Um_register rc)
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
static inline void sstore (MemorySegments_T memorySegments, 
                           Registers_T registers, Um_register ra,
                           Um_register rb, Um_register rc)
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
static inline void add    (Registers_T registers, Um_register ra, 
                           Um_register rb, Um_register rc) 
{
        uint32_t rbVal = Registers_get(registers, rb);
        uint32_t rcVal = Registers_get(registers, rc);  

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
static inline void mul    (Registers_T registers, Um_register ra, 
                           Um_register rb, Um_register rc)
{
        uint32_t rbVal = Registers_get(registers, rb);
        uint32_t rcVal = Registers_get(registers, rc);

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
static inline void divide (Registers_T registers, Um_register ra, 
                           Um_register rb, Um_register rc)
{
        uint32_t rbVal = Registers_get(registers, rb);
        uint32_t rcVal = Registers_get(registers, rc);  

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
static inline void nand   (Registers_T registers, Um_register ra, 
                           Um_register rb, Um_register rc)
{
        uint32_t rbVal = Registers_get(registers, rb);
        uint32_t rcVal = Registers_get(registers, rc);

        Registers_put(registers, ra, ~(rbVal & rcVal));
}

/*
 * halt()
 * purpose: stops um execution and ends program
 * parameters: UMachine_T (machine to be stopped)
 * return type: void
 * errors thrown: none
 */
static inline void halt(UMachine_T um)
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
static inline void map    (MemorySegments_T memorySegments, 
                           Registers_T registers, Um_register rb, 
                           Um_register rc)
{
        uint32_t nWords = Registers_get(registers, rc);
        uint32_t segmentId = MemorySegments_map(memorySegments, nWords, 
                                                INIT_VAL); 

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
static inline void unmap  (MemorySegments_T memorySegments, 
                           Registers_T registers, Um_register rc)
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
static inline void out    (Registers_T registers, Um_register rc)
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
static inline void in     (Registers_T registers, Um_register rc)
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
static inline void loadp  (MemorySegments_T memorySegments, 
                           Registers_T registers, uint32_t *programCounter, 
                           Um_register rb, Um_register rc)
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
static inline void loadval(Registers_T registers, Um_register ra, unsigned val)
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
static inline void Machine_free(UMachine_T *um)
{
        // assert(um != NULL && *um != NULL);
        UMachine_T machine = *um;
        MemorySegments_free(&((machine) -> memorySegments));
        Registers_free(&((machine) -> registers));
        free(machine);
}

/*
 * Machine_storeInstructions()
 * purpose: stores the instructions for a machine program
 * parameters: UMachine_T (machine representation), void *(pointer to
 *             sequence of instructinos)
 * return type: voide
 * errors thrown: assertion exception if um is NULL 
 */
void Machine_storeInstructions(UMachine_T um, void *instructions, 
                               int32_t seg0size)
{
        // assert(um != NULL);
        MemorySegments_T memorySegments = um -> memorySegments; 
        MemorySegments_storeSegment0(memorySegments, instructions, seg0size); 
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
static inline void Machine_performInstruction(UMachine_T um, 
                                              uint32_t instruction, 
                                              int32_t opcodeWidth, 
                                              int32_t registerWidth)
{
        int32_t opcode, ra, rb, rc;
        uint32_t value; 
        Registers_T registers;

        // assert (um != NULL); 

        registers = um -> registers; 

        opcode = shr(shl(instruction, 64 - (opcodeWidth + (INSTRUCTION_SIZE - 
                         opcodeWidth))), 64 - opcodeWidth); 

        ra = shr(shl(instruction, 64 - (registerWidth + (registerWidth * 2))),
                     64 - registerWidth); 
        rb = shr(shl(instruction, 64 - (registerWidth + registerWidth)),
                     64 - registerWidth); 
        rc = shr(shl(instruction, 64 - registerWidth),
                     64 - registerWidth);
        
        switch (opcode) {
                case CMOV:
                        cmov(registers, ra, rb, rc);
                        (um -> programCounter)++;
                        break;
                case SLOAD:
                        sload(um -> memorySegments, registers, ra, rb, rc);
                        (um -> programCounter)++;
                        break;
                case SSTORE:
                        sstore(um -> memorySegments, registers, ra, rb, rc);
                        (um -> programCounter)++;
                        break;
                case ADD:
                        add(registers, ra, rb, rc);
                        (um -> programCounter)++;
                        break;
                case MUL:
                        mul(registers, ra, rb, rc);
                        (um -> programCounter)++;
                        break;
                case DIV:
                        divide(registers, ra, rb, rc);
                        (um -> programCounter)++;
                        break;
                case NAND:
                        nand(registers, ra, rb, rc);
                        (um -> programCounter)++;
                        break;
                case HALT:
                        halt(um);
                        break;
                case MAP:
                        map(um -> memorySegments, registers, rb, rc);
                        (um -> programCounter)++;
                        break;
                case UNMAP:
                        unmap(um -> memorySegments, registers, rc); 
                        (um -> programCounter)++;
                        break;
                case OUTPUT:
                        out(registers, rc); 
                        (um -> programCounter)++;
                        break;
                case INPUT:
                        in(registers, rc); 
                        (um -> programCounter)++;
                        break;
                case LOADP:
                        loadp(um -> memorySegments, registers, &(um -> 
                        programCounter), 
                      rb, rc); 
                        break;
                case LV:
                        ra = shr(shl(instruction, 64 - (registerWidth + 
                                     INSTRUCTION_SIZE - registerWidth - 
                                     opcodeWidth)), 64 - registerWidth); 
                        value = shr(shl(instruction, 64 - LOADVAL_WIDTH),
                                        64 - LOADVAL_WIDTH);
                        loadval(registers, ra, value);
                        (um -> programCounter)++;
                        break;
                default:
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

        // assert(um != NULL);
        while (true) {
                instruction = MemorySegments_getSegment0(um -> memorySegments, 
                                                         um -> programCounter);
                Machine_performInstruction(um, instruction, OPCODE_WIDTH, 
                                           REGISTER_WIDTH);
        }
}

/*
 * storeInstructions(char *filename, uint32_t instrSize)
 * purpose: parses the instructions out of a file and returns them 
 *          in a contiguous sequence in memory
 * parameters: char *(filename), int32_t (number of bytes in an instruction)
 * return type: void *(pointer to sequence of instructions (UArray_T))
 * errors thrown: assertion exceptions if file can't be opened, an error
 *                occurs while reading file, or the number of instructions
 *                in the file is not even
 */
void *storeInstructions(char *filename, int32_t instrSize)
{
        uint32_t *instructions;
        int32_t nInstructions;

        FILE *infile = fopen(filename, READ_MODE);
        if (infile == NULL) {
                fprintf(stderr, "CANNOT OPEN FILE\n");
                exit(EXIT_FAILURE); 
        }

        nInstructions = getInstructionCount(filename, instrSize);
        instructions = malloc(nInstructions * instrSize);
        assert(instructions != NULL);
        parseInstructions(infile, instructions, instrSize, nInstructions);

        fclose(infile);
        return instructions;
}

/*
 * parseInstructions()
 * purpose: parses instructions from an infile and places them in a UArray_T
 * parameters: FILE *(file to parse instructions from), UArray_T (data 
 *             structure to hold parsed instructions), int32_t (number of
 *             bytes in an instruction), int32_t (number of instructions in
 *             file)
 * return type: void
 * errors thrown: none
 */
static inline void parseInstructions(FILE *infile, uint32_t *instructions, 
                              int32_t instrSize, int32_t nInstructions)
{
        int32_t i, j;
        uint64_t instr, byte;
        int32_t nInstrBits = instrSize * BITS_IN_BYTE;

        for (i = 0; i < nInstructions; i++) {

                for (j = 1; j <= instrSize; j++) {
                        uint32_t lsb = nInstrBits - (j * BITS_IN_BYTE);
                        
                        byte = fgetc(infile);
                        assert(!(feof(infile)) && !(ferror(infile)));
                        
                        unsigned hi = BITS_IN_BYTE + nInstrBits - (j * 
                                      BITS_IN_BYTE);
                        instr = shl(shr(instr, hi), hi) 
                                | shr(shl(instr, 64 - lsb), 64 - lsb)
                                | (byte << lsb);
                }

                *(instructions + i) = instr;
                instr = 0;
        }
}

/*
 * getInstructionCount()
 * purpose: calculates the number of instructions in a file
 * parameters: char *(filename), int32_t (the number of bytes per 
 *             instruction)
 * return type: int32_t (number of instructions in a file)
 * errors thrown: assertion exception if the file does not contain an even
 *                number of instructions or if the file cannot be opened
 */
int32_t getInstructionCount(char *filename, int32_t instrSize)
{
        struct stat fileStats;

        stat(filename, &fileStats);
        if (fileStats.st_size % instrSize != 0) {
                fprintf(stderr, "INVALID # OF INSTRUCTIONS\n");
                exit(EXIT_FAILURE);
        }

        return (fileStats.st_size / instrSize);
}

/*
 * output()
 * purpose: outputs byte on standard output 
 * parameters: uint8_t (a byte to be ouptutted to standard output)
 * return type: void
 * errors thrown: assertion error when an error occurs in putchar() to stdout
 */
static inline void output(uint8_t byte)
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
static inline uint32_t input()
{
        uint32_t ch = getchar();
        assert(!ferror(stdin));

        return ch;
}

/*
 * MemorySegments_new()
 * purpose: creates a new instance of a MemorySegments_T
 * parameters: int32_t (desired length of segment 0)
 * return type: MemorySegment_T (pointer to the memory segment instance)
 * errors thrown: none
 */
static inline MemorySegments_T MemorySegments_new(int32_t segment0length)
{       
        MemorySegments_T memorySegments = malloc(sizeof(*memorySegments));
        assert(memorySegments != NULL);
        
        memorySegments -> segments = calloc(DEFAULT_HINT, sizeof(uint32_t *));
        assert(memorySegments -> segments != NULL);
        memorySegments->numSegments = 0; 
        memorySegments->segmentSizes = malloc(sizeof(uint32_t) * DEFAULT_HINT);
        assert(memorySegments -> segmentSizes != NULL);
        memorySegments->segmentCapacity = DEFAULT_HINT;

        memorySegments->unmappedIndices = malloc(sizeof(uint32_t) * 
                                                 DEFAULT_HINT);
        assert(memorySegments -> unmappedIndices != NULL);
        memorySegments->unmappedCapacity = DEFAULT_HINT; 
        memorySegments->unmappedSize = 0; 

        memorySegments->segment0 = malloc(segment0length * sizeof(uint32_t));
        assert(memorySegments -> segment0 != NULL);
        *(memorySegments->segmentSizes) = segment0length;
        
        return memorySegments;
}

/*
 * MemorySegments_free()
 * purpose: frees any memory on the heap used by a MemorySegments_T instance
 * parameters: MemorySegments_T *(double pointer to a memory segments 
 *             representation)
 * return type: void
 * errors thrown: assertion exception if memorySegments is NULL or points 
 *                to NULL
 */
static inline void MemorySegments_free(MemorySegments_T *memorySegments)
{
        // assert(memorySegments != NULL && *memorySegments != NULL);

        free((*memorySegments)->segment0);
        free((*memorySegments)->segmentSizes);
        
        free_segments((*memorySegments)->segments, 
                        (*memorySegments)->segmentCapacity);
        free((*memorySegments)->unmappedIndices);

        free(*memorySegments);
}

/*
 * free_segments()
 * purpose: frees memory used by a segments sequence
 * parameters: Seq_T *(pointer to a sequence of segments to free)
 * return type: void
 * errors thrown: none
 */
static inline void free_segments(uint32_t **segments, int32_t segmentCapacity)
{
        uint32_t *segment;
        int32_t i; 

        for (i = 0; i < segmentCapacity; i++) {
                segment = *(segments + i);
                if ((segment) != NULL) {
                        free(segment); 
                }
        }
        
        free(segments);
}

/*
 * MemorySegments_map()
 * purpose: creates a new segment holding nWords number of words, each word 
 *          initialized to initVal. The new segment is stored in 
 *          memorySegments and its id is returned from the function
 * parameters: MemorySegments_T (memory segment instance), int32_t (number
 *             of words to go in new segment), uint32_t (value to be put
 *             in each word of segment)
 * return type: int32_t (the id of the new memorySegment)
 * errors thrown: assertion exception if memorySegments is NULL
 */
static inline int32_t MemorySegments_map(MemorySegments_T memorySegments, 
                                         int32_t nWords, uint32_t initVal)
{
        int32_t segmentId;
        uint32_t *newSegment;
        bool expanded = false;


        newSegment = new_segment(nWords, initVal);
        
        
        if (memorySegments -> unmappedSize > 0) {
                segmentId = reuseSegmentId(memorySegments->unmappedIndices, 
                                           &(memorySegments->segments), 
                                           newSegment, 
                                           &(memorySegments->unmappedSize)); 
        } else {
                segmentId = newSegmentId(&(memorySegments->segments), 
                                         newSegment, 
                                         memorySegments -> numSegments, 
                                         memorySegments -> segmentCapacity, 
                                         &expanded);
        }

        int32_t segmentCapacity = memorySegments -> segmentCapacity; 
        uint32_t **segmentSizes = &(memorySegments -> segmentSizes);
        
        if (expanded) {
                int32_t newCapacity = segmentCapacity * 2;
                *segmentSizes = realloc(*segmentSizes, 
                                       (size_t)newCapacity * sizeof(uint32_t));
                assert(*segmentSizes != NULL);
                memorySegments -> segmentCapacity = newCapacity;
        }
        
        *((memorySegments -> segmentSizes) + segmentId) = nWords;
        
        (memorySegments -> numSegments) += 1; 
        return segmentId;
}

/*
 * new_segment()
 * purpose: creates a new segment with the capacity to hold nWords number of
 *          words, each word being initialized to intiVal
 * parameters: int32_t (number of words for segment to hold), uint32_t (value
 *             to initialize segment's words to)
 * return type: UArray_T (new segment)
 * errors thrown: none
 */
static inline uint32_t *new_segment(int32_t nWords, uint32_t initVal)
{
        int32_t i;
        uint32_t *newSegment = malloc(nWords * sizeof(uint32_t));
        assert(newSegment != NULL);
        
        for (i = 0; i < nWords; i++) {
                *(newSegment + i) = initVal;
        }

        return newSegment;
}

/*
 * reuseSegmentId()
 * purpose: reuses a segment id by assigning that id to a new segment
 * parameters: Seq_T (sequence of unmapped indices), Seq_T (sequence of 
 *             mapped segments), UArray_T (segment to be mapped)
 * return type: int32_t (newly mapped segment's id)
 * errors thrown: none
 */
static inline int32_t reuseSegmentId(int32_t *unmappedIndices,
                                     uint32_t ***segments, uint32_t *newSegment,
                                     int32_t *unmappedSize)
{
        int32_t segmentIndex = *(unmappedIndices + *unmappedSize - 1);

        *(*segments + segmentIndex) = newSegment; 

        *unmappedSize -= 1;

        return getSegmentId(segmentIndex);
}

/*
 * newSegmentId()
 * purpose: creates a new segment id for and maps a new segment with that id
 * parameters: Seq_T (sequence of mapped segments), UArray_T (segment to be 
 *             mapped)
 * return type: int32_t (newly mapped segment's id)
 * errors thrown: none
 */
static inline int32_t newSegmentId(uint32_t ***segments, uint32_t *newSegment, 
                                   int32_t numSegments, int32_t segmentCapacity,
                                   bool *expanded)
{
        if (numSegments >= (segmentCapacity) - 1) {
                int32_t newCapacity = segmentCapacity * 2; 
                
                *segments = realloc(*segments, newCapacity * 
                                    sizeof(uint32_t *));
                assert(*segments != NULL);

                for (int32_t i = segmentCapacity; i < newCapacity; i++) {
                        *(*segments + i) = NULL; 
                }
                
                *expanded = true;
        }

        *(*segments + numSegments) = newSegment; 

        return getSegmentId(numSegments);
}

/*
 * MemorySegments_unmap()
 * purpose: unmaps a segment with segmentId in memorySegments
 * parameters: MemorySegments_T (memory segments representation),
 *             int32_t (memory segment id)
 * return type: void
 * errors thrown: assertion exception if memorySegments is NULL
 */
static inline void MemorySegments_unmap(MemorySegments_T memorySegments, 
                                        int32_t segmentId)
{
        uint32_t *segment;
        int32_t segmentIndex = getSegmentIndex(segmentId);
        
        // assert(memorySegments != NULL);
        uint32_t **segments = memorySegments -> segments; 
        segment = *(segments + segmentIndex); 
        // assert(segment != NULL);

        saveUnmappedIndex(&(memorySegments->unmappedIndices), segmentIndex, 
                          &(memorySegments -> unmappedSize), 
                          &(memorySegments -> unmappedCapacity));

        free(segment);
        *(segments + segmentIndex) = NULL; 
        (memorySegments -> numSegments) -= 1;
}

/*
 * saveUnmappedIndex()
 * purpose: stores an unmapped segment index
 * parameters: Seq_T (sequence of unmapped indices), int32_t (segment index
 *             that was newly unmapped)
 * return type: void
 * errors thrown: none
 */
static inline void saveUnmappedIndex(int32_t **unmappedIndices, 
                                     int32_t segmentIndex, 
                                     int32_t *unmappedSize, 
                                     int32_t *unmappedCapacity)
{
        // assert(segmentIndex_p != NULL);

        if (*unmappedSize >= *unmappedCapacity - 1) {
                int32_t newCapacity = *unmappedCapacity * 2;
                *unmappedIndices = realloc(*unmappedIndices, sizeof(int32_t) *
                                           newCapacity);
                assert(*unmappedIndices != NULL);
                *unmappedCapacity = newCapacity; 
        }
 
        *(*unmappedIndices + *unmappedSize) = segmentIndex;   
        *unmappedSize += 1;  
}

/*
 * MemorySegments_put()
 * purpose: puts a value in memorySegments's segment with segmentId 
 *          at index i within the segment
 * parameters: MemorySegments_T (memory segment representation), 
 *             int32_t (id of the segment), int32_t (index in the segment to
 *             put value), uint32_t (value to put in segment)
 * return type: void
 * errors thrown: assertion exception if memorySegments is NULL
 */
static inline void MemorySegments_put(MemorySegments_T memorySegments, int32_t 
segmentId, int32_t i, uint32_t val)
{
        uint32_t *segment;

        // assert(memorySegments != NULL);
        uint32_t **segments = memorySegments -> segments; 
        segment = *(segments + getSegmentIndex(segmentId)); 

        *(segment + i) = val;
}

/*
 * MemorySegments_get()
 * purpose: returns a value in memorySegment's segment with segmentId 
 *          at segmentIndex
 * parameters: MemorySegments_T (memory segment representation), 
 *             int32_t (segment id), int32_t (index of word within segment)
 * return type: uint32_t (word at index in segment)
 * errors thrown: assertion exception if memorySegments is NULL
 */
static inline uint32_t MemorySegments_get(MemorySegments_T memorySegments, 
                                          int32_t segmentId, int32_t i)
{
        uint32_t *segment;

        // assert(memorySegments != NULL);
        uint32_t **segments = memorySegments -> segments; 
        segment = *(segments + getSegmentIndex(segmentId)); 

        return *(segment + i);
}

/*
 * MemorySegments_storeSegment0()
 * purpose: puts instructions into segment 0 of memorySegments
 *          void *instructions is expected to be of type UArray_T
 * parameters: MemorySegments_T (memory segments representation), 
 *             void *(UArray_T holding instructions)
 * return type: void
 * errors thrown: assertion exception if memorySegments is 0
 */
static inline void MemorySegments_storeSegment0(MemorySegments_T memorySegments,
                                  void *instructions, int32_t seg0size)
{
        // assert(memorySegments != NULL && instructions != NULL);
        free((memorySegments)->segment0);
        memorySegments->segment0 = (uint32_t *)instructions;
        *(memorySegments->segmentSizes) = seg0size;
}

/*
 * MemorySegments_getSegment0()
 * purpose: returns the value stored in segment 0 at a certain index within
 *          the segment
 * parameters: MemorySegments_T (memory segments representation),
 *             int32_t (index within segment 0 to access)
 * return type: uint32_t (value in segment 0 at index i)
 * errors thrown: assertion exception if memorySegments is NULL or if 
 *                segmentIndex is out of bounds
 */
static inline uint32_t MemorySegments_getSegment0(
                                               MemorySegments_T memorySegments,
                                               int32_t i)
{   
        // assert(memorySegments != NULL);
        uint32_t *segment0 = memorySegments->segment0; 
        return *(segment0 + i);
}

/*
 * MemorySegments_putSegment0()
 * purpose: puts a value into segment 0
 * parameters: MemorySegments_T (memory segments representation), int32_t
 *             (index of segment 0 to place value in), uint32_t (value)
 * return type: void
 * errors thrown: assertion exception of MemorySegments_T is NULL
 */
static inline void MemorySegments_putSegment0(MemorySegments_T memorySegments, 
                                              int32_t i, uint32_t val)
{
        // assert(memorySegments != NULL);
        uint32_t *segment0 = memorySegments->segment0; 
        *(segment0 + i) = val; 
}

/*
 * MemorySegments_switchSegment0()
 * purpose: replaces segment 0 with a copy of another segment
 * parameters: MemorySegments_T (memory segments representation),
 *             uint32_t (segment id of replacement segment)
 * return type: void
 * errors thrown: assertino exception if memroySegments is NULL
 */
static inline void MemorySegments_switchSegment0(
                                               MemorySegments_T memorySegments,
                                               uint32_t segmentId)
{
        uint32_t i;
        uint32_t nWords = *(memorySegments -> segmentSizes + segmentId);
        uint32_t *newSegment0; 
        uint32_t *newSegment0copy; 
        uint32_t **segments = memorySegments -> segments; 
        
        //assert(memorySegments != NULL);
        
        newSegment0 = *(segments + getSegmentIndex(segmentId));
        
        newSegment0copy = malloc(nWords * sizeof(uint32_t));
        assert(newSegment0copy != NULL);
        
        for (i = 0; i < nWords; i++) {
                *(newSegment0copy + i) = *(newSegment0 + i); 
        }
        
        free((memorySegments)->segment0); 
        memorySegments -> segment0 = newSegment0copy; 
        *(memorySegments -> segmentSizes) = nWords;
} 

/* 
 * Registers_new()
 * purpose: initializes an instance of Registers_T with nRegisters registers  
 *          and each register holding a copy of initVal
 * parameters: int32_t (the number of registers desired), uint32_t (the value
 *             to put in each register)
 * return type: Registers_T (pointer to registers instance)
 * errors thrown: none
 */
static inline Registers_T Registers_new(int32_t nRegisters, uint32_t initVal)
{
        int32_t i = 0;
        uint32_t *registerArray;
        Registers_T registers;

        registerArray = malloc(nRegisters * sizeof(uint32_t));
        assert(registerArray != NULL);
        registers = malloc(sizeof(*registers));
        assert(registers != NULL);

        // assert(registers != NULL);

        for (i = 0; i < nRegisters; i++) {
                *(registerArray + (i)) = initVal;
        }

        registers->size = nRegisters;
        registers->registerArray = registerArray;

        return registers;
}

/* 
 * Registers_free()
 * purpose: destructs an instance of a Registers_T, freeing all of its memory
 *          on the heap
 * parameters: Registers_T *(double pointer to a registers instance to free)
 * return type: void
 * errors thrown: assertion exceptoin if registers is NULL or points to NULL 
 */
static inline void Registers_free(Registers_T *registers)
{
        // assert(registers != NULL && *registers != NULL);
        free((*registers)->registerArray);
        free(*registers);
        *registers = NULL;
}

/* 
 * Registers_get()
 * purpose: retrieves the memory stored at register i 
 * parameters: Registers_T (pointer to the registers instance), int32_t (the
 *             index at which to get data from)
 * return type: uint32_t (the data at the register index)
 * errors thrown: assertion exception if registers is NULL or if i is 
 *                outside of scope
 */
static inline uint32_t Registers_get(Registers_T registers, int32_t i)
{
        // assert(registers != NULL);
        uint32_t *registerArray = registers -> registerArray; 
        return *(registerArray + (i));
}

/* 
 * Registers_put()
 * purpose: stores a value in register i of the Registers_T instance 
 * parameters: Registers_T (pointer to the registers instance), 
 *             int32_t (index of the register to put val in)
 *             uint32_t (val to be put in index of register)
 * return type: none
 * errors thrown: assertion exception if registers is NULL or i is outside 
 *                of scope
 */
static inline void Registers_put(Registers_T registers, int32_t i, uint32_t val)
{
        // assert(registers != NULL);
        uint32_t *registerArray = registers -> registerArray; 
        *(registerArray + (i)) = val;
}

static inline uint64_t shl(uint64_t word, unsigned bits)
{
        // assert(bits <= 64);
        if (bits == 64)
                return 0;
        else
                return word << bits;
}

static inline uint64_t shr(uint64_t word, unsigned bits)
{
        // assert(bits <= 64);
        if (bits == 64)
                return 0;
        else
                return word >> bits;
}

#undef getSegmentId
#undef getSegmentIndex


