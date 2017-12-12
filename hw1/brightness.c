/*
 *     brightness.c
 *     by Ethan Chen (echen11) and Genevieve Silverman (gsilve02), 9/15/17
 *     HW1 Part A - Brightness
 *
 *     Brightness is a program that can take in a pnm file and print the
 *     average brightness of all the pixels in the image to stdout.
 *     Brightness can be used by specifying a pnm file name on the
 *     command line or a pnm file can be inputted through standard input.
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pnmrdr.h"
#include "mem.h"
#include "except.h"

#define STDIN_ARGC 1
#define COMMAND_LINE_ARGC 2

double avg_pix_brightness(FILE *fp);
void create_gray_map_reader(FILE *fp, Pnmrdr_T *rdr);
double calc_avg_pix_brightness(Pnmrdr_T *rdr, Pnmrdr_mapdata mapdata);

Except_T Null_File_E = { "Unable to open file" };
Except_T Empty_File_E = { "No pixels in file" };

int main(int argc, char *argv[])
{
        double avg_brightness;

        if (argc == STDIN_ARGC) {
                avg_brightness = avg_pix_brightness(stdin);
        } else if (argc == COMMAND_LINE_ARGC) {
                avg_brightness = avg_pix_brightness(fopen(argv[1], "rd"));
        } else {
                fprintf(stderr, "provide only one filename\n");
                return EXIT_FAILURE;
        }

        fprintf(stdout, "%.3f\n", avg_brightness);
        exit(EXIT_SUCCESS);
}

/* 
 * return the avg pixel brightness in a gray map image
 * it is a C.R.E. for the file to not be open
 */
double avg_pix_brightness(FILE *fp)
{
        Pnmrdr_T rdr;
        double avg_brightness;

        if (fp == NULL || ferror(fp)) {
                RAISE(Null_File_E);
        }

        create_gray_map_reader(fp, &rdr);
        avg_brightness = calc_avg_pix_brightness(&rdr, Pnmrdr_data(rdr));

        fclose(fp);
        Pnmrdr_free(&rdr);
        return avg_brightness;
}

/*
 * create a gray map image reader instance
 * it is a C.R.E. for the image in fp to not be a graymap
 */
void create_gray_map_reader(FILE *fp, Pnmrdr_T *rdr)
{
        *rdr = Pnmrdr_new(fp);

        if (Pnmrdr_data(*rdr).type != Pnmrdr_gray) {
                fprintf(stderr, "brightness: file type was not a gray map\n");
                exit(EXIT_FAILURE);
        }
}

/*
 * calculate the avg pixel brightness
 * it is a C.R.E. for the picture in the rdr to have 0 pixels
 */
double calc_avg_pix_brightness(Pnmrdr_T *rdr, Pnmrdr_mapdata mapdata)
{
        int i, curr_pixel, num_pixels = mapdata.height * mapdata.width;
        double curr_pixel_brightness, avg_brightness = 0.0;

        if (num_pixels < 1) {
                RAISE(Empty_File_E);
        }

        for (i = 0; i < num_pixels; i++) {
                curr_pixel = Pnmrdr_get(*rdr);
                curr_pixel_brightness = 
                                   curr_pixel / (double)mapdata.denominator;

                avg_brightness += curr_pixel_brightness / num_pixels;
        }

        return avg_brightness;
}

#undef STDIN_ARGC
#undef COMMAND_LINE_ARGC
