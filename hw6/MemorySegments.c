/*
 * MemorySegments.c
 * Ethan Chen and Genevieve Silverman
 * HW 6 - UM
 *
 * MemorySegments.c is the implementation of an interface that allows one to 
 * interact with segments of memory through mapping and unmapping segments 
 * and getting and putting values into specified segment indices.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <seq.h>
#include <uarray.h>
#include <assert.h>
#include "MemorySegments.h"

/* conversions between segment index and segment id */
#define getSegmentId(segmentIndex) segmentIndex + 1
#define getSegmentIndex(segmentId) segmentId - 1

static const int DEFAULT_HINT = 500; 

/* data abstraction for memory segments */
struct MemorySegments_T {
        UArray_T segment0;
        Seq_T segments;
        Seq_T unmappedIndices;
};

/* helper functions for MemorySegments_free() */
static void     free_segments       (Seq_T *segments);
static void     free_unmappedIndices(Seq_T *unmappedIndices);

/* helper functions for MemorySegment_map() */
static UArray_T new_segment         (int32_t nWords, uint32_t initVal);
static int32_t  reuseSegmentId      (Seq_T unmappedIndices, Seq_T segments, 
                                     UArray_T newSegment);
static int32_t  newSegmentId        (Seq_T segments, UArray_T newSegment);

/* helper functions for MemorySegment_unmap() */
static void     saveUnmappedIndex   (Seq_T unmappedIndices, 
                                     int32_t segmentIndex);

/*
 * MemorySegments_new()
 * purpose: creates a new instance of a MemorySegments_T
 * parameters: int32_t (desired length of segment 0)
 * return type: MemorySegment_T (pointer to the memory segment instance)
 * errors thrown: none
 */
MemorySegments_T MemorySegments_new(int32_t segment0length)
{
        MemorySegments_T memorySegments = malloc(sizeof(*memorySegments));
        assert(memorySegments != NULL);

        memorySegments->segments = Seq_new(DEFAULT_HINT);
        memorySegments->unmappedIndices = Seq_new(DEFAULT_HINT);
        memorySegments->segment0 = UArray_new(segment0length, 
                                              sizeof(uint32_t));
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
void MemorySegments_free(MemorySegments_T *memorySegments)
{
        assert(memorySegments != NULL && *memorySegments != NULL);

        UArray_free(&((*memorySegments)->segment0));
        free_segments(&((*memorySegments)->segments));
        free_unmappedIndices(&((*memorySegments)->unmappedIndices));

        free(*memorySegments);
}

/*
 * free_segments()
 * purpose: frees memory used by a segments sequence
 * parameters: Seq_T *(pointer to a sequence of segments to free)
 * return type: void
 * errors thrown: none
 */
static void free_segments(Seq_T *segments)
{
        UArray_T segment;

        while (Seq_length(*segments) > 0) {
                segment = (UArray_T)Seq_remhi(*segments);
                if (segment != NULL) {
                        UArray_free(&segment);
                }
        }
        Seq_free(segments);
}

/*
 * free_unmappedIndices()
 * purpose: frees memory used by an unmappedIndices sequence
 * parameters: Seq_T *(pointer to a sequence of unmappedIndices to free)
 * return type: void
 * errors thrown: none
 */
static void free_unmappedIndices(Seq_T *unmappedIndices)
{
        int32_t *unmappedIndex;

        while (Seq_length(*unmappedIndices) > 0) {
                unmappedIndex = (int32_t *)Seq_remhi(*unmappedIndices);
                if (unmappedIndex != NULL) {
                        free(unmappedIndex);
                }
        }
        Seq_free(unmappedIndices);
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
int32_t MemorySegments_map(MemorySegments_T memorySegments, int32_t nWords, 
                           uint32_t initVal)
{
        int32_t segmentId;
        UArray_T newSegment;

        assert(memorySegments != NULL);

        newSegment = new_segment(nWords, initVal);

        if (Seq_length(memorySegments->unmappedIndices) > 0) {
                segmentId = reuseSegmentId(memorySegments->unmappedIndices, 
                                           memorySegments->segments, 
                                           newSegment);
        } else {
                segmentId = newSegmentId(memorySegments->segments, newSegment);
        }
        
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
static UArray_T new_segment(int32_t nWords, uint32_t initVal)
{
        uint32_t *val;
        int32_t i;
        UArray_T newSegment = UArray_new(nWords, sizeof(uint32_t));

        for (i = 0; i < UArray_length(newSegment); i++) {
                val = (uint32_t *)UArray_at(newSegment, i);
                *val = initVal;
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
static int32_t reuseSegmentId(Seq_T unmappedIndices, Seq_T segments, 
                              UArray_T newSegment)
{
        int32_t *segmentIndex_p, segmentIndex;

        segmentIndex_p = (int32_t *)Seq_remhi(unmappedIndices);
        segmentIndex = *segmentIndex_p;

        free(segmentIndex_p);
        Seq_put(segments, segmentIndex, newSegment);

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
static int32_t newSegmentId(Seq_T segments, UArray_T newSegment)
{
        int32_t segmentIndex = Seq_length(segments);
        Seq_addhi(segments, newSegment);
        return getSegmentId(segmentIndex);
}

/*
 * MemorySegments_unmap()
 * purpose: unmaps a segment with segmentId in memorySegments
 * parameters: MemorySegments_T (memory segments representation),
 *             int32_t (memory segment id)
 * return type: void
 * errors thrown: assertion exception if memorySegments is NULL
 */
void MemorySegments_unmap(MemorySegments_T memorySegments, int32_t segmentId)
{
        UArray_T segment;
        int32_t segmentIndex = getSegmentIndex(segmentId);
        
        assert(memorySegments != NULL);

        segment = (UArray_T)Seq_get(memorySegments->segments, segmentIndex);
        assert(segment != NULL);

        saveUnmappedIndex(memorySegments->unmappedIndices, segmentIndex);

        UArray_free(&segment);
        Seq_put(memorySegments->segments, segmentIndex, NULL);
}

/*
 * saveUnmappedIndex()
 * purpose: stores an unmapped segment index
 * parameters: Seq_T (sequence of unmapped indices), int32_t (segment index
 *             that was newly unmapped)
 * return type: void
 * errors thrown: none
 */
static void saveUnmappedIndex(Seq_T unmappedIndices, int32_t segmentIndex)
{
        int32_t *segmentIndex_p = malloc(sizeof(*segmentIndex_p));
        assert(segmentIndex_p != NULL);

        *segmentIndex_p = segmentIndex;
        Seq_addhi(unmappedIndices, segmentIndex_p);     
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
void MemorySegments_put(MemorySegments_T memorySegments, int32_t segmentId, 
                        int32_t i, uint32_t val)
{
        UArray_T segment;

        assert(memorySegments != NULL);

        segment = (UArray_T)Seq_get(memorySegments->segments, 
                                    getSegmentIndex(segmentId));
        *(uint32_t *)UArray_at(segment, i) = val;
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
uint32_t MemorySegments_get(MemorySegments_T memorySegments, int32_t segmentId,
                            int32_t i)
{
        UArray_T segment;

        assert(memorySegments != NULL);

        segment = (UArray_T)Seq_get(memorySegments->segments, 
                                    getSegmentIndex(segmentId));
        return *(int32_t *)UArray_at(segment, i);
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
void MemorySegments_storeSegment0(MemorySegments_T memorySegments, 
                                  void *instructions)
{
        assert(memorySegments != NULL && instructions != NULL);
        UArray_free(&(memorySegments->segment0));
        memorySegments->segment0 = (UArray_T)instructions;
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
uint32_t MemorySegments_getSegment0(MemorySegments_T memorySegments, 
                                    int32_t i)
{   
        assert(memorySegments != NULL);
        return *(uint32_t *)UArray_at(memorySegments->segment0, i);
}

/*
 * MemorySegments_putSegment0()
 * purpose: puts a value into segment 0
 * parameters: MemorySegments_T (memory segments representation), int32_t
 *             (index of segment 0 to place value in), uint32_t (value)
 * return type: void
 * errors thrown: assertion exception of MemorySegments_T is NULL
 */
void MemorySegments_putSegment0(MemorySegments_T memorySegments, int32_t i, 
                                uint32_t val)
{
        uint32_t *value_p;
        assert(memorySegments != NULL);
        
        value_p = (uint32_t *)UArray_at(memorySegments -> segment0, 
                                                  i);
        *value_p = val;
}

/*
 * MemorySegments_switchSegment0()
 * purpose: replaces segment 0 with a copy of another segment
 * parameters: MemorySegments_T (memory segments representation),
 *             uint32_t (segment id of replacement segment)
 * return type: void
 * errors thrown: assertino exception if memroySegments is NULL
 */
void MemorySegments_switchSegment0(MemorySegments_T memorySegments, 
                                   uint32_t segmentId)
{       
        UArray_T newSegment0; 

        assert(memorySegments != NULL);
        
        newSegment0 = (UArray_T)Seq_get(memorySegments->segments, 
                                        getSegmentIndex(segmentId));
        UArray_free(&(memorySegments -> segment0)); 
        memorySegments -> segment0 = UArray_copy(newSegment0, 
                                                 UArray_length(newSegment0)); 
} 

#undef getSegmentId
#undef getSegmentIndex