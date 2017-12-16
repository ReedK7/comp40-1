/*
 * MemorySegments.h
 * Ethan Chen and Genevieve Silverman
 * HW 6 - UM
 *
 * MemorySegments.h is an interface that allows one to interact with 
 * segments of memory through mapping and unmapping segments and getting and
 * putting values into specified segment indices.
 */

#ifndef MEMORYSEGMENTS_INCLUDED
#define MEMORYSEGMENTS_INCLUDED

#include <stdint.h>

typedef struct MemorySegments_T *MemorySegments_T;

/*
 * creates a new instance of a MemorySegments_T
 * no C.R.E.s
 */
MemorySegments_T MemorySegments_new(int32_t segment0length);

/*
 * frees any memory on the heap used by a MemorySegments_T instance
 * C.R.E. if memorySegments is NULL or points to NULL
 */
void MemorySegments_free(MemorySegments_T *memorySegments);

/*
 * creates a new segment holding nWords number of words, each word 
 * initialized to initVal. The new segment is stored in memorySegments and
 * its id is returned from the function
 * C.R.E. if memorySegments is NULL
 */
int32_t MemorySegments_map(MemorySegments_T memorySegments, int32_t nWords, 
                           uint32_t initVal);

/*
 * unmaps a segment with segmentId in memorySegments
 * C.R.E.s if memorySegments is NULL or if segmentId is unmapped already
 */
void MemorySegments_unmap(MemorySegments_T memorySegments, int32_t segmentId);

/*
 * puts a value in memorySegments's segment with segmentId at segmentIndex
 * C.R.E. if memorySegments is NULL or if segmentId is unmapped
 */
void MemorySegments_put(MemorySegments_T memorySegments, int32_t segmentId, 
                        int32_t i, uint32_t val);

/*
 * returns a value in memorySegment's segment with segmentId at segmentIndex
 * C.R.E. if memorySegments is NULL or if segmentId is unmapped
 */
uint32_t MemorySegments_get(MemorySegments_T memorySegments, int32_t segmentId,
                            int32_t i);

/*
 * puts instructions into segment 0 of memorySegments
 * C.R.E. if memorySegments is NULL
 */
void MemorySegments_storeSegment0(MemorySegments_T memorySegments, 
                                void *instructions);

/*
 * returns the value stored in segment 0 at a certain index within
 * the segment
 * C.R.E. if memorySegments is NULL or if segmentIndex is out of bounds
 */
uint32_t MemorySegments_getSegment0(MemorySegments_T memorySegments, 
                                    int32_t i);

/*
 * puts a value into segment 0
 * C.R.E. if memorySegments is NULL
 */
void MemorySegments_putSegment0(MemorySegments_T memorySegments, int32_t i, 
                                uint32_t val);

/*
 * puts a value into segment 0
 * C.R.E. replaces segment 0 with a copy of another segment
 */
void MemorySegments_switchSegment0(MemorySegments_T memorySegments, 
                                   uint32_t segmentId); 

#endif
