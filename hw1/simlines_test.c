/*
 *     readaline.c
 *     by Ethan Chen (echen11) and Genevieve Silverman (gsilve02), 9/15/17
 *     HW1 Part B - Simlines
 *
 *     A file to test our simlines program
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include "readaline.h"
#include "except.h"
#include "simlines.h"
#include "atom.h"

void printTestResults(char *test, int numErrors);
/* void testIsWordChar();
void testParseWords(); */

int main(int argc, char *argv[]) 
{
        simlines(argc, argv);

        /* (void)argc;
        (void)argv;

        testIsWordChar();
        testParseWords(); */

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

/*void testIsWordChar()
{
        int numErrors = 0;
        char *test = "is word char";

        if (isWordChar('\0')) {
                numErrors++;
        }
        if (isWordChar('%')) {
                numErrors++;
        }
        if (isWordChar(' ')) {
                numErrors++;
        }
        if (isWordChar('\n')) {
                numErrors++;
        }
        if (!isWordChar('a')) {
                numErrors++;
        }
        if (!isWordChar('A')) {
                numErrors++;
        }
        if (!isWordChar('_')) {
                numErrors++;
        }
        if (!isWordChar('0')) {
                numErrors++;
        }
        if (isWordChar('!')) {
                numErrors++;
        }
        if (isWordChar(-1)) {
                numErrors++;
        }

        printTestResults(test, numErrors);
}

void testParseWords()
{
        int numBytes = 30;
        char *datap =  malloc(numBytes);
        char *expectedParsedWord = "hello goodbye what is up";
        int numErrors = 0;
        char *test = "parse words";


        strcpy(datap, "hello,goodbye&what!is****up\n");
        parseWords(&datap, numBytes);
        if (strcmp(datap, expectedParsedWord) != 0) {
                fprintf(stderr, "HERE 88\n");
                numErrors++;
        }

        strcpy(datap, "**.,/';yo      ");
        expectedParsedWord = "yo";
        numBytes = strlen(datap);
        parseWords(&datap, numBytes);
        if (strcmp(datap, expectedParsedWord) != 0) {
                fprintf(stderr, "%s\n", datap);
                fprintf(stderr, "%s\n", expectedParsedWord);
                numErrors++;
        }

        datap = "\0\0hi\0\0";
        expectedParsedWord = "hi";
        numBytes = 5;
        parseWords(&datap, numBytes);
        if (strcmp(datap, expectedParsedWord) != 0) {
                fprintf(stderr, "%s\n", datap);
                fprintf(stderr, "%s\n", expectedParsedWord);
                numErrors++;
        }

        strcpy(datap, "            ");
        expectedParsedWord = "";
        numBytes = strlen(datap);
        parseWords(&datap, numBytes);
        if (strcmp(datap, expectedParsedWord) != 0) {
                fprintf(stderr, "%s\n", datap);
                fprintf(stderr, "%s\n", expectedParsedWord);
                numErrors++;
        }

        strcpy(datap, "\n\n\n\n");
        expectedParsedWord = "";
        numBytes = strlen(datap);
        parseWords(&datap, numBytes);
        if (strcmp(datap, expectedParsedWord) != 0) {
                fprintf(stderr, "%s\n", datap);
                fprintf(stderr, "%s\n", expectedParsedWord);
                numErrors++;
        }
*/
        /* char *datap2 = malloc(1);
        for (int i = 0; i < 32; i++) {
                char c = i;
                strcpy(datap2, &c);
                parseWords(&datap2, 1);
                if (strcmp(datap2, "") != 0) {
                        fprintf(stderr, "%s\n", datap2);
                        fprintf(stderr, "%s\n", "");
                        numErrors++;
                }
        }

        char c = 127;
        strcpy(datap2, &c);
        parseWords(&datap2, 1);
        if (strcmp(datap2, "") != 0) {
                fprintf(stderr, "%s\n", datap2);
                fprintf(stderr, "%s\n", "");
                numErrors++;
        }

        free(datap2); */

/*        free(datap);
        printTestResults(test, numErrors);
} */








