/*
 * MemorySegment_test.c
 * Ethan Chen and Genevieve Silverman
 * HW 6 - UM
 *
 * MemorySegment_test.c holds unit tests for the MemorySegments module
 * (MemorySegments.h & MemorySegments.c). The test results will be 
 * printed on standard output.
 *
 * compile with command: $ make MemorySegments_test
 * execute with command: $ ./MemorySegments_test
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <except.h>
#include <uarray.h>
#include "MemorySegments.h"


static const uint32_t N_TEST_ITERATIONS = 15;

/* map tests */
static void testMapWithoutSegment0();
static void testMapContinuous();
static void testMapUncontinuous();
static void testMapInitVal();

/* unmap tests */
static void testUnmapContinuous();
static void testUnmapUnmapped();
static void testUnmapNull();

/* put and get tests */
static void testPutAndGet();
static void testPutAndGetNull();
static void testPutAndGetUnmappedId();
static void testPutAndGetSegment0();

/* segment 0 tests (store and get) */
static void testStoreSegment0Null();
static void testGetSegment0Null();
static void testGetSegment0InvalidIndex();
static void testStoreAndGetSegment0();


void printErrors(int numErrors, char *test);

int main()
{
        printf("   **** memory segments tests ****\n");

        /* map tests */
        testMapWithoutSegment0();
        testMapContinuous();
        testMapUncontinuous();
        testMapInitVal();

        /* unmap tests */
        testUnmapContinuous();
        testUnmapUnmapped();
        testUnmapNull();

        /* put and get tests */
        testPutAndGet();
        testPutAndGetNull();
        testPutAndGetUnmappedId();
        testPutAndGetSegment0();

        /* store and get segment 0 tests */
        testStoreSegment0Null();
        testGetSegment0Null();
        testStoreAndGetSegment0();
        testGetSegment0InvalidIndex();

        return 0;
}

/*
 * this test makes sure that segment 0 is never mapped by 
 * MemorySegments_map() because segment 0 exists in a separate data 
 * structure from the other segments inside of the MemorySegment_T 
 * instance
 */
static void testMapWithoutSegment0()
{
        int numErrors = 0;
        uint32_t segmentId, i;
        MemorySegments_T memorySegments = MemorySegments_new(1);

        for (i = 0; i < N_TEST_ITERATIONS; i++) {
                segmentId = MemorySegments_map(memorySegments, 1, 0);
                if (segmentId == 0) {
                        numErrors++;
                }
        }

        MemorySegments_free(&memorySegments);

        printErrors(numErrors, "Map without segment 0:\t\t");
}

/*
 * this test checks that each segment mapped continuously has a 
 * segment id that is 1 greater than the previous segment because
 * of the invariant in our MemorySegment implementation that ensures
 * that segments mapped one after another will occur next to each other
 * in memory. This invariant is not true if a segment is unmapped in
 * between mapping segments (to be tested below)
 */
static void testMapContinuous() 
{
        int numErrors = 0;
        uint32_t segmentId, prevSegmentId = 0, i;
        MemorySegments_T memorySegments = MemorySegments_new(1);

        for (i = 0; i < N_TEST_ITERATIONS; i++) {
                segmentId = MemorySegments_map(memorySegments, 1, 0); 
                if (segmentId != prevSegmentId + 1) {

                        numErrors++;
                }
                prevSegmentId = segmentId;
        }

        MemorySegments_free(&memorySegments);

        printErrors(numErrors, "Map continuous:\t\t\t");
}

/*
 * tests that the when a segment gets unmapped, that segment's id is
 * the id where the next segment that's mapped will be put
 */
static void testMapUncontinuous()
{
        int numErrors = 0;
        uint32_t segmentId, unmappedSegmentId = N_TEST_ITERATIONS / 2, i;
        MemorySegments_T memorySegments = MemorySegments_new(1);

        for (i = 0; i < N_TEST_ITERATIONS; i++) {
                segmentId = MemorySegments_map(memorySegments, 1, 0);
        }

        MemorySegments_unmap(memorySegments, unmappedSegmentId);

        segmentId = MemorySegments_map(memorySegments, 1, 0);

        if (segmentId != unmappedSegmentId) {
                numErrors++;
        }

        MemorySegments_free(&memorySegments);

        printErrors(numErrors, "Map uncontinuous:\t\t");
}

/*
 * tests that mapped segments have expected init value stored in them
 */
static void testMapInitVal()
{
        int numErrors = 0;
        uint32_t initVal = 1, i;
        MemorySegments_T memorySegments = MemorySegments_new(1);

        for (i = 0; i < N_TEST_ITERATIONS; i++) {
                MemorySegments_map(memorySegments, 1, initVal);
        }

        /* check that init val 0 was put into segment and can be retrieved */
        for (i = 1; i <= N_TEST_ITERATIONS; i++) {
                if (MemorySegments_get(memorySegments, i, 0) != initVal) {
                        fprintf(stderr, "val gotten was %d\n", 
                                MemorySegments_get(memorySegments, i, 0));
                        numErrors++;
                }
        }

        MemorySegments_free(&memorySegments);

        printErrors(numErrors, "Map init val:\t\t\t");
}

/*
 * tests that unmapping works as expected when unmapping contiguous segments
 */
static void testUnmapContinuous()
{
        int numErrors = 0;
        uint32_t i;
        MemorySegments_T memorySegments = MemorySegments_new(1);

        for (i = 0; i < N_TEST_ITERATIONS; i++) {
                MemorySegments_map(memorySegments, 1, 0);
        }

        for (i = 1; i < N_TEST_ITERATIONS; i++) {
                MemorySegments_unmap(memorySegments, i);

                TRY
                        MemorySegments_get(memorySegments, i, 0);
                        numErrors++;
                ELSE
                        ;
                END_TRY;
        }

        MemorySegments_free(&memorySegments);

        printErrors(numErrors, "Unmap continuous:\t\t");
}

/*
 * tests that an unmapped segment cannot be unmapped again before it's been
 * re-mapped
 */
static void testUnmapUnmapped()
{
        int numErrors = 0;
        uint32_t i;
        MemorySegments_T memorySegments = MemorySegments_new(1);

        for (i = 0; i < N_TEST_ITERATIONS; i++) {
                MemorySegments_map(memorySegments, 1, 0);
        }

        for (i = 1; i < N_TEST_ITERATIONS; i++) {
                MemorySegments_unmap(memorySegments, i);

                TRY
                        MemorySegments_unmap(memorySegments, i);
                        numErrors++;
                ELSE
                        ;
                END_TRY;
        }

        MemorySegments_free(&memorySegments);

        printErrors(numErrors, "Unmap unmapped:\t\t\t");
}

/*
 * tests that MemorySegments_unmap() throws a C.R.E. when passed a NULL
 * memorySegments instance
 */
static void testUnmapNull()
{
        int numErrors = 0;

        TRY
                MemorySegments_unmap(NULL, 0);
                numErrors++;
        ELSE
                ;
        END_TRY;

        printErrors(numErrors, "Unmap NULL:\t\t\t");
}

/*
 * tests that when a value is put into a mapped segment at a certain index,
 * that the value can then be retrieved through MemorySegments_get()
 */
static void testPutAndGet()
{
        int numErrors = 0;
        uint32_t i, segmentId = N_TEST_ITERATIONS / 2, val = N_TEST_ITERATIONS;
        MemorySegments_T memorySegments = MemorySegments_new(1);

        /* test 1 - put and get one value */
        for (i = 0; i < N_TEST_ITERATIONS; i++) {
                MemorySegments_map(memorySegments, 1, 0);
        }

        MemorySegments_put(memorySegments, segmentId, 0, val);
        if (MemorySegments_get(memorySegments, segmentId, 0) != val) {
                numErrors++;
        }

        /* test 2 - put and get multiple values */
        for (i = 1; i <= N_TEST_ITERATIONS; i++) {
                MemorySegments_put(memorySegments, i, 0, i);
        }

        for (i = 1; i <= N_TEST_ITERATIONS; i++) {
                if (MemorySegments_get(memorySegments, i, 0) != i) {
                        numErrors++;
                }
        }

        MemorySegments_free(&memorySegments);

        printErrors(numErrors, "Put and Get:\t\t\t");
}

/*
 * tests that MemorySegments_put() and MemorySegments_get() throw a 
 * run time error when passed a null MemorySegment_T
 */
static void testPutAndGetNull()
{
        int numErrors = 0;

        TRY
                MemorySegments_put(NULL, 0, 0, 0);
                numErrors++;
        ELSE 
                ;
        END_TRY;


        TRY
                MemorySegments_get(NULL, 0, 0);
                numErrors++;
        ELSE 
                ;
        END_TRY;

        printErrors(numErrors, "Put and Get NULL:\t\t");
}

/*
 * tests that MemorySegment_put() MemorySegment_get() cannot be called on an 
 * unmapped segment Id
 */
static void testPutAndGetUnmappedId()
{
        int numErrors = 0;
        uint32_t segmentId = N_TEST_ITERATIONS + 1;
        MemorySegments_T memorySegments = MemorySegments_new(1);

        TRY
                MemorySegments_put(memorySegments, segmentId, 0, 0);
                numErrors++;
        ELSE
                ;
        END_TRY;


        TRY
                MemorySegments_get(memorySegments, segmentId, 0);
                numErrors++;
        ELSE
                ;
        END_TRY;

        MemorySegments_free(&memorySegments);

        printErrors(numErrors, "Put and Get unmapped id:\t");      
}

/*
 * tests that MemorySegment_put() and MemorySegment_get() cannot be called on
 * segment 0
 */
static void testPutAndGetSegment0()
{
        int numErrors = 0;
        uint32_t i, segmentId = 0;
        MemorySegments_T memorySegments = MemorySegments_new(1);

        for (i = 0; i < N_TEST_ITERATIONS; i++) {
                MemorySegments_map(memorySegments, 1, 0);
        }

        TRY
                MemorySegments_put(memorySegments, segmentId, 0, 0);
                numErrors++;
        ELSE
                ;
        END_TRY;


        TRY
                MemorySegments_get(memorySegments, segmentId, 0);
                numErrors++;
        ELSE
                ;
        END_TRY;

        MemorySegments_free(&memorySegments);

        printErrors(numErrors, "Put and Get segment 0:\t\t");
}

/*
 * tests that memorySegments and instructions cannot be a NULL parameter for 
 * MemorySegments_storeSegment0() 
 */
static void testStoreSegment0Null()
{
        int numErrors = 0;
        UArray_T uarray = UArray_new(1, 1);
        MemorySegments_T memorySegments = MemorySegments_new(1);

        TRY
                MemorySegments_storeSegment0(NULL, uarray);
                numErrors++;
        ELSE ;
        END_TRY;
        UArray_free(&uarray);

        TRY
                MemorySegments_storeSegment0(memorySegments, NULL);
                numErrors++;
        ELSE ;
        END_TRY;

        MemorySegments_free(&memorySegments);

        printErrors(numErrors, "Store Segment 0 NULL:\t\t");
}

/*
 * tests that memorySegments cannot be a NULL parameter for 
 * MemorySegments_getSegment0() 
 */
static void testGetSegment0Null()
{
        int numErrors = 0;

        TRY
                MemorySegments_getSegment0(NULL, 0);
                numErrors++;
        ELSE
                ;
        END_TRY;

        printErrors(numErrors, "Get Segment 0 NULL:\t\t");
}

/*
 * tests that it's a C.R.E. for the segmentIndex to be out of bounds
 * when calling MemorySegments_getSegment0()
 */
static void testGetSegment0InvalidIndex()
{
        int numErrors = 0;
        UArray_T uarray = UArray_new(1, 1);
        MemorySegments_T memorySegments = MemorySegments_new(1);

        MemorySegments_storeSegment0(memorySegments, uarray);

        TRY
                MemorySegments_getSegment0(memorySegments, 1);
                numErrors++;
        ELSE
                ;
        END_TRY;

        MemorySegments_free(&memorySegments);

        printErrors(numErrors, "Get Segment 0 invalid index:\t");
}

/*
 * tests that MemorySegments_storeSegment0() and MemorySegment_getSegment0()
 * work as expected
 */
static void testStoreAndGetSegment0()
{
        int numErrors = 0;
        uint32_t *val, i;
        UArray_T uarray = UArray_new(N_TEST_ITERATIONS, sizeof(uint32_t));
        MemorySegments_T memorySegments = MemorySegments_new(1);

        for (i = 0; i < N_TEST_ITERATIONS; i++) {
                val = (uint32_t *)UArray_at(uarray, i);
                *val = i;
        }

        MemorySegments_storeSegment0(memorySegments, uarray);

        for (i = 0; i < N_TEST_ITERATIONS; i++) {
                if (MemorySegments_getSegment0(memorySegments, i) != i) {
                        numErrors++;
                }
        }

        MemorySegments_free(&memorySegments);

        printErrors(numErrors, "Store and Get Segment 0:\t");
}

/*
 * prints to standard output the results of a unit test 
 */
void printErrors(int numErrors, char *test)
{
        if (numErrors == 0) {
                printf("%sPASSED\n", test);
        } else if (numErrors == 1) {
                printf("%sFAILED - %d error\n", test, numErrors);
        } else {
                printf("%sFAILED - %d error\n", test, numErrors);
        }
}

