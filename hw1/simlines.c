/*
 *     simlines.c
 *     by Ethan Chen (echen11) and Genevieve Silverman (gsilve02), 9/15/17
 *     HW1 Part C - Simlines
 *
 *     Simlines can process multiple files and print any lines in files
 *     with the same contiguous sequence of words regardless of any 
 *     extraneous characters on the line.
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
#include "table.h"
#include "matchgroup.h"

/**************************** constants *****************************/
#define INIT_TABLE_SIZE 2500
#define WORD_SEPARATOR ' '
#define FILE_NAME_LENGTH 20


/******************** private function prototypes *******************/

/* read and index lines from file */
void index_files(int num_files, char *filenames[], Table_T line_table); 
void read_file(char **filename, Table_T line_table);
void add_line_occurrence(char **filename, int line_num, 
                         const char *atom, Table_T line_table);

/* process chars */
void parse_words(char **line, int string_len);
void add_terminating_char(char **stringpp, int *num_chars);
bool is_word_char(char ch);
bool is_underscore(char ch);

/* identifies matchgroups with more than one occurrence location */
void count_repeat(const void *key, void **value, void *cl);
void count_repeated_lines(Table_T line_table, int *num_repeats);

/* print keys and values */
void print_simlines(Table_T line_table);
void print_key_and_value(const void *key, void **value, void *cl);

/* destructors */
void remove_value(const void *key, void **value, void *cl);
void deallocate(Table_T line_table);   



/**************************** exceptions ****************************/

Except_T Atom_Null_E = { "A null atom was made" };
Except_T Realloc_E = { "Error reallocating memory" };



/********************** main program function ***********************/

/* runs the simlines program */
void simlines(int num_files, char *filenames[]) 
{       
        Table_T line_table = Table_new(INIT_TABLE_SIZE, NULL, NULL);
        index_files(num_files, filenames, line_table);
        print_simlines(line_table);

        deallocate(line_table);
}



/*********************** read file functions ************************/

/* indexes the lines in files */
void index_files(int num_files, char *filenames[], Table_T line_table)
{
        int i;
        for (i = 1; i < num_files; i++) {
                read_file(&(filenames[i]), line_table); 
        }

}

void read_file(char **filename, Table_T line_table)
{
        const char *atom_string;
        char *datap;
        int line_num = 0;
        size_t string_len;
        FILE *fp;

        fp = fopen(*filename, "r");  
        string_len = readaline(fp, &datap);

        while (datap != NULL) {
                parse_words(&datap, string_len);
                line_num++;

                /* only create atom is string isn't empty after parsing */
                if (strlen(datap) > 0) {
                        atom_string = Atom_string(datap);
                        add_line_occurrence(filename, line_num, 
                                            atom_string, line_table);
                }

                free(datap);
                string_len = readaline(fp, &datap);
        }
        fclose(fp);
}

/* adds an occurence of a line to the line_table */
void add_line_occurrence(char **filename, int line_num, 
                         const char *atom, Table_T line_table)
{
        int *line_nump = malloc(sizeof(line_num));
        Matchgroup_p mg = Table_get(line_table, atom);
        *line_nump = line_num;

        if (mg == NULL) {
                mg = init_matchgroup(filename, line_nump); 
        } else {
                add_occurrence(mg, filename, line_nump);
        }

        Table_put(line_table, atom, mg);
}



/********************* process chars functions **********************/

/* takes in a string and removes non-word characters, separating contiguous
 * word characters by one space */
void parse_words(char **line, int string_len) 
{
        int i;
        char ch;

        int num_char_kept = 0;
        int last_space_index = -1;

        char *words_line = malloc(string_len + 1);

        for (i = 0; i < string_len; i++) {
                ch = *(*line + i);
                if (is_word_char(ch)) {
                        *(words_line + num_char_kept) = ch;
                        num_char_kept++;
                } else {
                        /* ignore non-word chars unless it comes right
                         * after a word char --> add space separator */
                        if (last_space_index < num_char_kept - 1) {
                                *(words_line + num_char_kept) = WORD_SEPARATOR;
                                last_space_index = num_char_kept;
                                num_char_kept++;
                        }
                }
        }
        add_terminating_char(&words_line, &num_char_kept);

        /* realloc because parsed string is likely smaller */
        words_line = realloc(words_line, num_char_kept);

        free(*line);
        *line = words_line;
}

/* add a null character to terminate a character pointer */
void add_terminating_char(char **stringpp, int *num_chars)
{
        /* replace a terminating space with a null char */
        if (*num_chars > 0 && *(*stringpp + *num_chars- 1) == ' ') {
                *(*stringpp + *num_chars - 1) = (char)'\0';

        /* add a null char */
        } else {
                *(*stringpp + *num_chars) = (char)'\0';
                *num_chars = *num_chars + 1;
        }
}

bool is_word_char(char ch)
{
        if (isalpha(ch) || isdigit(ch) || is_underscore(ch)) {
                return true;
        }

        return false;
}

bool is_underscore(char ch)
{
        if (ch == '_') {
                return true;
        }

        return false;
}



/********************* identify repeated lines **********************/

/* counts a repeated line */
void count_repeat(const void *key, void **value, void *cl)
{
        (void)key;

        int *num_repeats = cl;
        if (occurs_more_than_once(*value)) {
                *num_repeats = *num_repeats + 1;
        }
}

void count_repeated_lines(Table_T line_table, int *num_repeats)
{
        Table_map(line_table, count_repeat, num_repeats);
}



/*********************** prints keys & values ***********************/

/* prints only repeated lines to stdout */
void print_simlines(Table_T line_table) 
{
        int num_pairs = 0;
        count_repeated_lines(line_table, &num_pairs);

        Table_map(line_table, print_key_and_value, &num_pairs);
}


/* prints a key and a value to stdout with formatting */
void print_key_and_value(const void *key, void **value, void *cl)
{
        Matchgroup_p mg = *value;
        char *filename;
        int line_num;
        int i;
        int num_occurrences = get_count(mg);
        int *num_pairs_leftp = cl;

        if (occurs_more_than_once(*value)) {

                printf("%s\n", (char *)key);

                for (i = 0; i < num_occurrences; i++) {
                        filename = get_filename(mg, i);
                        line_num = get_line_num(mg, i);

                        if (strlen(filename) <= FILE_NAME_LENGTH) {
                                printf("%-20s %7d\n", filename, line_num);
                        } else {
                                printf("%-20s%7d\n", filename, line_num);
                        }
                }

                if (*num_pairs_leftp > 1) {
                        printf("\n");
                }
                *num_pairs_leftp = *num_pairs_leftp - 1;
        }
}



/*************************** destructors ****************************/

void remove_value(const void *key, void **value, void *cl)
{
        (void)key;
        (void)cl;

        matchgroup_free(*value);
        free(*value);
}

void deallocate(Table_T line_table)
{
        Table_map(line_table, remove_value, NULL);
        Table_free(&line_table);
}

/* undefine global constants */
#undef INIT_TABLE_SIZE
#undef WORD_SEPARATOR

