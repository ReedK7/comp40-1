#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "bitpack.h"
#include "assert.h"

Except_T Bitpack_Overflow = {"Overflow packing bits"};

bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        /* Make sure width is correct */
        assert (width != 0 && width <= 64);
        uint64_t temp = n; 
        /* If shifting off 64-width bits doesn't change the value,
            then the value fits in width bits. */
        temp = temp << (64-width);
        temp = temp >> (64-width);
        return (temp == n);
}

bool Bitpack_fitss( int64_t n, unsigned width)
{
        assert (width != 0 && width <= 64);
        int64_t temp = n; 
        /* If shifting off 64-width bits doesn't change the value,
            then the value fits in width bits. */
        temp = temp << (64-width);
        temp = temp >> (64-width);
        return (temp == n);
}
 
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert (width + lsb <= 64 && width <= 64);
        if (width == 0) {
                return 0;
        }
        /* We shift off bits to the left of the value we're getting,
                and then those to the right of the value. */
        word = word << (64 - (width + lsb));
        word = word >> (64 - width);
        return word;
}

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert (width + lsb <= 64 && width <= 64);
        if (width == 0) {
                return 0;
        }
        int64_t num;
        /* We shift off bits to the left of the value we're getting,
                and then those to the right of the value. */
        num  = word << (64 - (width + lsb));
        num = num >> (64 - width);
        return num;
}

uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                      uint64_t value)
{
        assert (width <= 64 && width + lsb <= 64); 

        /* If the value doesn't fit width, then we have a CRE */
        if (Bitpack_fitsu(value, width) == 0) {
              RAISE (Bitpack_Overflow);  
              exit (EXIT_FAILURE);
        }

        /* Make the mask be all 1's, except with 0's in
            the lsb to lsb+width bits. We can then 0 out 
            these bits in word */
        uint64_t mask = ~(0);
        mask = mask << (64-(width+lsb));
        mask = mask >> (64-width);
        mask = mask << lsb;
        word = word & (~mask);

        /* Shift value to the correct bits, and place it in
            the zeroed out bits of word. */
        value = value << lsb;
        return (word | value);
}

uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,  
                     int64_t value)
{
        assert (width <= 64 && width + lsb <= 64); 
        /* If the value doesn't fit width, then we have a CRE */
        if (Bitpack_fitss(value, width) == 0) {
              RAISE (Bitpack_Overflow);  
              exit(EXIT_FAILURE);
        }

        /* Make the mask be all 1's, except with 0's in
            the lsb to lsb+width bits. We can then 0 out 
            these bits in word */
        uint64_t mask = ~(0);
        mask = mask << (64-(width+lsb));
        mask = mask >> (64-width);
        mask = mask << lsb;
        word = word & (~mask);

        /* Shift value to the correct bits, and place it in
            the zeroed out bits of word. */
        uint64_t n_value = (uint64_t)value;
        n_value = value << (64-width);
        n_value = n_value >> (64-(width+lsb));
        return (word | n_value);
}




