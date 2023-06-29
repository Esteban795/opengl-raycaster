#ifndef RAY_H
#define RAY_H


#include "map.h"

struct Ray {
    double depth;
    unsigned int color;
    double angle_of_incidence;
};

enum Side {
    SOUTH,
    NORTH,
    EAST,
    WEST
};

typedef struct Ray ray;

ray cast_ray(map m,position pos,double ray_angle,double player_angle);
#endif