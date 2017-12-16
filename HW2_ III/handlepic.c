#include "handlepic.h"


Except_T Not_Valid_Pnm_E = {"Not a valid Pnm file"};


Bit2_T save_bits (Pnmrdr_T pic)
{       
        Pnmrdr_mapdata data = Pnmrdr_data(pic);
        int height = data.height; 
        int width = data.width;
        Bit2_T bitmap = Bit2_new(width, height); 
        /* Iterate through Bit2 and put each pixel from pic */ 
        for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                        Bit2_put(bitmap, col, row, Pnmrdr_get(pic));
                }
        }
        return bitmap;
}

Pnmrdr_T check_valid(FILE *fp, int argc) 
{
        Pnmrdr_T pic = Pnmrdr_new(fp);
        Pnmrdr_mapdata data = Pnmrdr_data (pic);
         if (data.type != Pnmrdr_bit) {
                close_and_free(fp, argc, pic); 
                RAISE (Not_Valid_Pnm_E); 
         } 
         return pic; 
}

void close_and_free(FILE *fp, int argc, Pnmrdr_T pic) 
{
         Pnmrdr_free(&pic);
         if (argc == 2) {
                 fclose(fp);
         }
}




