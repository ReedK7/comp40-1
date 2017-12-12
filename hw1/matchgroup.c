/*
 *     matchgroup.c
 *     by Ethan Chen (echen11) and Genevieve Silverman (gsilve02), 9/15/17
 *     HW1 Part C - Simlines
 *
 *     Matchgroups are structures that can hold information about 
 *     multiple occurences of things (up to the user) in files. The
 *     matchgroup interface allows users to create, manipulate and 
 *     process matchgroups in order to keep track of occurences in files.
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h> 
#include "except.h"
#include "matchgroup.h"
#include "seq.h"

#define INIT_SEQ_SIZE 100

/* exceptions */
Except_T Index_Out_of_Range_E = { "Index out of range" };
Except_T Null_Pointer_E = { "Parameter was null" };
Except_T MAlloc_E = { "Error allocating memory" };

/* matchgroup data structure */
struct Matchgroup {
        int count;
        Seq_T filenames;
        Seq_T line_nums;
};


/*
 * it is a C.R.E. to pass in null parameters or when malloc fails
 */
Matchgroup_p init_matchgroup(char **filename, int *line_num) 
{       
        Matchgroup_p mg;

        if (filename == NULL || line_num == NULL) {
                RAISE (Null_Pointer_E);
        }

        mg = malloc(sizeof(*mg));

        if (mg == NULL) {
                RAISE(MAlloc_E);
        }

        mg -> count = 1;
        mg -> filenames = Seq_new(INIT_SEQ_SIZE);
        mg -> line_nums = Seq_new(INIT_SEQ_SIZE);
        
        Seq_addhi(mg -> filenames, filename);
        Seq_addhi(mg -> line_nums, line_num); 

        return mg; 
}

/*
 * it is a C.R.E. to pass in a null matchgroup
 */
void matchgroup_free(Matchgroup_p mg)
{
        int i;
        int *line_nump;

        if (mg == NULL) {
                RAISE(Null_Pointer_E);
        }

        for (i = 0; i < mg -> count; i++) {
                line_nump = Seq_remhi(mg -> line_nums);
                free(line_nump);
        }

        Seq_free(&(mg -> line_nums));
        Seq_free(&(mg -> filenames));
}

/* 
 * add occurrence location to an existing matchgroup
 * it is a C.R.E. to pass in null parameters 
 */
void add_occurrence(Matchgroup_p mg, char **filename, int *line_num) 
{ 
        if (mg == NULL || filename == NULL || line_num == NULL) {
                RAISE(Null_Pointer_E);
        }

        Seq_addhi(mg -> filenames, filename);
        Seq_addhi(mg -> line_nums, line_num); 

        mg -> count++;  
}

/*
 * allows the caller to tell if the matchgroup contains a match at multiple
 * locations
 * it is a C.R.E. to pass in a null matchgroup
 */
bool occurs_more_than_once(Matchgroup_p mg)
{
        if (mg == NULL) {
                RAISE(Null_Pointer_E);
        }

        if (get_count(mg) > 1) {
                return true; 
        } else {  
                return false; 
        }
}

/*
 * it is a C.R.E. to pass in a null match group
 */
int get_count(Matchgroup_p mg) 
{
        if (mg == NULL) {
                RAISE(Null_Pointer_E);
        }

        return mg -> count; 
}

/*
 * return the (i + 1)th filename stored in a matchgroup
 * it is a C.R.E. to pass in a null match group or to specify an invalid
 * index
 */
char *get_filename(Matchgroup_p mg, int index)
{       
        if (mg == NULL) {
                RAISE(Null_Pointer_E);
        }

        if (index > Seq_length(mg -> filenames) || index < 0)  {
                RAISE(Index_Out_of_Range_E);
        }

        char **x = Seq_get(mg -> filenames, index);

        return *x;
}

/*
 * return the (i + 1)th line num stored in a matchgroup
 * it is a C.R.E. to pass in a null match group or to specify an invalid
 * index
 */
int get_line_num(Matchgroup_p mg, int index) 
{ 
        if (mg == NULL) {
                RAISE(Null_Pointer_E);
        }

        if (index > Seq_length(mg -> line_nums) || index < 0) {
                RAISE(Index_Out_of_Range_E);
        }

        int *x = Seq_get(mg -> line_nums, index);

        return *x;
}

#undef INIT_SEQ_SIZE
