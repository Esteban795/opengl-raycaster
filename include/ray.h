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


int get_ray_color(char symbol);

enum Side get_side_hit(int x, int y, int prev_x,int prev_y);

double get_ray_depth(position player_pos,int map_x, int map_y, double dx, double dy, double scale, enum Side side);

ray cast_ray(map m,position pos,double ray_angle,double player_angle);
#endif