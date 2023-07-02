#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdbool.h>

#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923
struct Map {
    char* world;
    int width;
    int height;
    double scale;
};

typedef struct Map* map;

struct Position {
    double x; 
    double y;
};

typedef struct Position position;

map create_map(char* world,int width,int height, double scale);

position get_player_pos(map m);

char* import_world(char path[],int* world_width,int* world_height);

#endif