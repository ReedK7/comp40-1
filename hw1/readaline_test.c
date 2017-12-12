/*
 *     readaline.c
 *     by Ethan Chen (echen11) and Genevieve Silverman (gsilve02), 9/15/17
 *     HW1 Part B - Readaline
 *
 *     A file to test our implementation of readaline
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include "readaline.h"
#include "except.h"

void printTestResults(char *test, int numErrors);
void testReadFileAtEOF();
void testWithNullParams();
void testReadOneNewLine();
void testOnlyNewLines();
void testWithOneSpace();
void testLongLines();
void testWithStdin();

int main()
{
        printf("testing readaline....\n");

        testReadFileAtEOF();
        testWithNullParams();
        testReadOneNewLine();
        testOnlyNewLines();
        testWithOneSpace();
        testLongLines();
        
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

void testReadFileAtEOF()
{
        char *datap, *filename = "empty.txt", *test = "read file at EOF";
        size_t size;
        FILE *fp;
        int numErrors = 0;

        fp = fopen(filename, "r");
        if (fp == NULL) {
                numErrors++;
                printTestResults(strcat("**file error**", filename), numErrors);
                return;
        }

        size = readaline(fp, &datap);

        if (size != 0) {
                numErrors++;
        }

        if (datap != NULL) {
                numErrors++;
        }

        free(datap);
        fclose(fp);
        printTestResults(test, numErrors);
}

void testWithNullParams()
{
        char *datap = NULL, *filename = "helloworld.txt",
             *test = "null function parameters";
        FILE *fp;
        int numErrors = 3;

        fp = fopen(filename, "r");
        if (fp == NULL) {
                numErrors = 1;
                printTestResults(strcat("**file error**", filename), numErrors);
                return;
        }

        TRY
                readaline(NULL, NULL);
        ELSE
                numErrors--;
        END_TRY;


        TRY
                readaline(fp, NULL);
        ELSE
                numErrors--;
        END_TRY;


        datap = "hi";
        TRY
                readaline(NULL, &datap);
        ELSE
                numErrors--;
        END_TRY;

        fclose(fp);
        printTestResults(test, numErrors);
}

void testReadOneNewLine()
{
        char *filename = "onenewline.txt", *datap,
             *test = "read file with one new line";
        FILE *fp;
        int numErrors = 0;
        size_t size;

        fp = fopen(filename, "r");
        if (fp == NULL) {
                numErrors++;
                printTestResults(strcat("**file error**", filename), numErrors);
                return;
        }

        size = readaline(fp, &datap);

        if (strlen(datap) != 1) {
                numErrors++;
        }

        if (size != 1) {
                numErrors++;
        }

        if (*datap != '\n') {
                numErrors++;
        }

        printTestResults(test, numErrors);
        free(datap);
        fclose(fp);
}

void testOnlyNewLines()
{
        int i, numNewLines = 4;
        char *filename = "newlines.txt", *datap,
             *test = "read file with only new lines";
        FILE *fp;
        int numErrors = 0;
        size_t size;

        fp = fopen(filename, "r");
        if (fp == NULL) {
                numErrors++;
                printTestResults(strcat("**file error**", filename), numErrors);
                return;
        }

        for (i = 0; i < numNewLines; i++) {
                size = readaline(fp, &datap);

                if (strlen(datap) != 1) {
                        numErrors++;
                }
                if (size != 1) {
                        numErrors++;
                }
                if (*datap != '\n') {
                        numErrors++;
                }
                if (numErrors > 0) {
                        i = numNewLines;
                }
                free(datap);
        }

        fclose(fp);
        printTestResults(test, numErrors);
}

void testWithOneSpace()
{
        char *filename = "space.txt", *datap,
             *test = "read file with one space";
        FILE *fp;
        int numErrors = 0;
        size_t size;

        fp = fopen(filename, "r");
        if (fp == NULL) {
                numErrors++;
                printTestResults(strcat("**file error**", filename), numErrors);
                return;
        }

        size = readaline(fp, &datap);

        if (strlen(datap) != 2) {
                numErrors++;
        }

        if (size != 2) {
                numErrors++;
        }

        if (*datap != ' ' || *(datap + 1) != '\n') {
                numErrors++;
        }

        printTestResults(test, numErrors);
        free(datap);
        fclose(fp);
}

void testLongLines()
{
        char *datap, *filename = "toolong.txt",
             *test = "exit with error when line too long";
        FILE *fp;
        int numErrors = 0;
        size_t size;
        size_t lineOneLen = 250, lineTwoLen = 300, lineThreeLen = 500;

        fp = fopen(filename, "r");
        if (fp == NULL) {
                numErrors++;
                printTestResults(strcat("**file error**", filename), numErrors);
                return;
        }

        size = readaline(fp, &datap);
        if (size != lineOneLen) {
                numErrors++;
        }
        if (strlen(datap) != lineOneLen) {
                numErrors++;
        }
        free(datap);

        size = readaline(fp, &datap);
        if (size != lineTwoLen) {
                numErrors++;
        }
        if (strlen(datap) != lineTwoLen) {
                numErrors++;
        }
        free(datap);

        size = readaline(fp, &datap);
        if (size != lineThreeLen) {
                numErrors++;
        }
        if (strlen(datap) != lineThreeLen) {
                numErrors++;
        }
        free(datap);

        fclose(fp);
        printTestResults(test, numErrors);
}

void testWithStdin()
{
        char *datap;
        char *test = "test with stdin";
        size_t size;
        int numErrors = 0;

        size = readaline(stdin, &datap);
        printf("readaline returned size is: %d\n", (int)size);

        if (datap == NULL) {
            numErrors++;
            printTestResults(test, numErrors);
            return;
        }

        printf("readaline set string to: %s\n", datap);
        free(datap);
}



