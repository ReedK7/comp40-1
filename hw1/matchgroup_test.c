/*
 *     matchgroup_test.c
 *     by Ethan Chen (echen11) and Genevieve Silverman (gsilve02), 9/15/17
 *     HW1 Part C - Simlines
 *
 *     A file to test our implementation of matchgroups
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h> 
#include "except.h"
#include "matchgroup.h"
#include "seq.h"

const int SIZE = 100;

void printTestResults(char *test, int numErrors);
void testGetCount(); 
void testaddOccurence();
void testoccursMoreThanOnce();
void testgetFileName();
void testgetLineNum(); 
void testConstructor();

int main() { 
        
        testGetCount();
        testaddOccurence();
        testoccursMoreThanOnce();
        testgetFileName();
        testgetLineNum();
        testConstructor();


        return EXIT_SUCCESS;

}

void printTestResults(char *test, int numErrors)
{
        if (numErrors > 0) {
                printf("FAILED: %s = %d ERRORS\n", test, numErrors);
        } else {
                printf("PASSED: %s\n", test);
        }
}

void testConstructor() 
{
        int numErrors = 0; 
        char *filename = "file3";
        int linenum = 1;
        char *test = "Constructor";
        void *mg;

        TRY
                initMatchGroup(NULL, NULL);
                numErrors++; 
        ELSE 
                ;
        END_TRY;

        mg = initMatchGroup(&filename, &linenum);
        if (getCount(mg) != 1) {
                numErrors++;
        }
        if (getFileName(mg, 0) != filename) {
                numErrors++;
        }
        if (getLineNum(mg, 0) != linenum) {
                numErrors++;
        }

        matchgroupFree(mg);
        free(mg);

        printTestResults(test, numErrors);
}

void testaddOccurence() 
{
        char *filename = "file3"; 
        int linenum = 1;  
        int numOccurences = 3;
        int numErrors = 0; 
        char *test = "Add Occurence"; 
        void *newMatch = initMatchGroup(&filename, &linenum);

        for (int i = 1; i < numOccurences; i++) {
                addOccurrence(newMatch, &filename, &linenum);
                if (getCount(newMatch) != i + 1) {
                        numErrors++;
                }
        }
        
        TRY 
               addOccurrence(newMatch, NULL, NULL);
               numErrors++;
        ELSE 
                ; 
        END_TRY; 


        TRY 
               addOccurrence(NULL, &filename, &linenum);
               numErrors++;
        ELSE 
                ; 
        END_TRY; 

        matchgroupFree(newMatch);
        free(newMatch);

        printTestResults(test, numErrors);
}

void testGetCount() 
{ 
        char *filename = "file3";
        int linenum = 1;  
        int numErrors = 0; 
        char *test = "Count Test"; 
        void *newMatch = initMatchGroup(&filename, &linenum);

        int count = getCount(newMatch); 

        if (count != 1) {
                numErrors++;
        }

        TRY
                getCount(NULL);
                numErrors++;
        ELSE
                ;
        END_TRY;

        matchgroupFree(newMatch);
        free(newMatch);

        printTestResults(test, numErrors);
}

void testoccursMoreThanOnce() 
{
        char *filename = "file3"; 
        int linenum = 1;  
        int numOccurences = 3;
        int numErrors = 0; 
        char *test = "Occurs More Than Once"; 
        void *newMatch = initMatchGroup(&filename, &linenum);

        for (int i = 1; i < numOccurences; i++) {
                addOccurrence(newMatch, &filename, &linenum);
                if (occursMoreThanOnce(newMatch) == false) 
                        numErrors++;  
        }

        TRY
                addOccurrence(NULL, &filename, &linenum);
                numErrors++;
        ELSE
                ;
        END_TRY;

        matchgroupFree(newMatch);
        free(newMatch);

        printTestResults(test, numErrors);

}

void testgetFileName()
{ 
        char *filename = "file3";
        int linenum = 1; 
        char *test = "Get File Name";
        int numErrors = 0;
        void *newMatch = initMatchGroup(&filename, &linenum);

        if (getFileName(newMatch, 0) != filename) {
                numErrors++;
        }
        TRY
                getFileName(newMatch, -1);
                numErrors++; 
        ELSE 
                ;
        END_TRY;


        TRY
                getFileName(NULL, -1);
                numErrors++; 
        ELSE 
                ;
        END_TRY;

        matchgroupFree(newMatch);
        free(newMatch);

        printTestResults(test, numErrors); 
}

void testgetLineNum() 
{ 
        char *filename = "file3";
        int linenum = 1; 
        char *test = "Get Line Num";
        int numErrors = 0;
        void *newMatch = initMatchGroup(&filename, &linenum);

        if (getLineNum(newMatch, 0) != linenum) {
                numErrors++;
        }

        TRY
                getLineNum(newMatch, -1);
                numErrors++; 
        ELSE 
                ;
        END_TRY;


        TRY
                getLineNum(NULL, -1);
                numErrors++; 
        ELSE 
                ;
        END_TRY;

        matchgroupFree(newMatch);
        free(newMatch);

        printTestResults(test, numErrors); 
}






