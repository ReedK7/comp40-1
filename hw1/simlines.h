/*
 *     simlines.h
 *     by Ethan Chen (echen11) and Genevieve Silverman (gsilve02), 9/15/17
 *     HW1 Part C - Simlines
 *
 *     Simlines can process multiple files and print any lines in files
 *     with the same contiguous sequence of words regardless of any 
 *     extraneous characters on the line.
 */ 

#ifndef SIMLINES_INCLUDED
#define SIMLINES_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include "readaline.h"
#include "except.h"
#include "matchgroup.h"
#include "table.h"

void simlines(int num_files, char *filenames[]);   

#endif