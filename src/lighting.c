#include "../include/lighting.h"

unsigned int colors_linear_interpolation(unsigned int c1, unsigned int c2, double val){
    unsigned int new_color = 0x000000;
    unsigned int mask = 0xFF;

    new_color |= (unsigned char) (((float)((c1 >> 16)&  mask) * (1-val)) + ((float)((c2 >> 16)&mask) * val));
    new_color <<= 8;
    new_color |= (unsigned char) (((float)((c1 >> 8)&mask) * (1-val)) + ((float)((c2 >> 8)&mask) * val));
    new_color <<= 8;
    new_color |= (unsigned char) (((float)((c1 >> 0)&mask) * (1-val)) + ((float)((c2 >> 0)&mask) * val));

    return new_color;
}