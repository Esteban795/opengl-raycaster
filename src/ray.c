#include "../include/ray.h"

int get_ray_color(char symbol){
    switch (symbol){
    case 'r':
        return 0xFF2222;
    case 'g':
        return 0x22FF22;
    case 'b':
        return 0x2222FF;
    case '#':
        return 0x111111;
    case 'w':
        return 0xEEEEEE;
    case 'a':
        return 0x22EEEE;
    default:
        return 0xF1F1F1;
    }
}

double get_incidence_angle(enum Side side,double angle){
    switch (side)
    {
    case WEST:
        return 2 * M_PI - angle;
    
    default:
        break;
    }
}