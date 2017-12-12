/*
 *     matchgroup.h
 *     by Ethan Chen (echen11) and Genevieve Silverman (gsilve02), 9/15/17
 *     HW1 Part C- Simlines
 *
 *     Matchgroups are structures that can hold information about 
 *     multiple occurences of things (up to the user) in files. The
 *     matchgroup interface allows users to create, manipulate and 
 *     process matchgroups in order to keep track of occurences in files.
 */ 

#ifndef MATCHGROUP_INCLUDED
#define MATCHGROUP_INCLUDED
#include "seq.h"

typedef struct Matchgroup *Matchgroup_p;
typedef enum { true = 1, false = 0 } bool;

/* it is a C.R.E. to pass in a null pointer into any function */

/* constructor and destructor */
Matchgroup_p init_matchgroup(char **filename, int *line_num);
void matchgroup_free(Matchgroup_p mg);

/* occurences */
void add_occurrence(Matchgroup_p mg, char **filename, int *line_num); 
bool occurs_more_than_once(Matchgroup_p mg); 

/* getter functions */
int get_count(Matchgroup_p mg);
char *get_filename(Matchgroup_p mg, int index);
int get_line_num(Matchgroup_p mg, int index);

#endif