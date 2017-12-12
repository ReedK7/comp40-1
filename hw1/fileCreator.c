/*
 *     fileCreator.c
 *     by Ethan Chen (echen11) and Genevieve Silverman (gsilve02), 9/15/17
 *     HW1 Part C - Simlines
 *
 *     This is a program we used to create files with special ASCII
 *     characters to test simlines with.
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>

int main() {
        FILE *fp; 

        fp = fopen("nullmiddle.txt", "w");
        fprintf(fp, "hi$@#*()!.,?#$&*+=hello&^#@!");

        fprintf(fp, "\n\0");

        fprintf(fp, "^&*#^hi()@@!@?/.<><>hello$#^&*"); 

        return EXIT_SUCCESS;
}