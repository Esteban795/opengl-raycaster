#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>

struct Map {
    char* world;
    int width;
    int height;
    double scale;
};

typedef struct Map* map;

enum Side { //enum to detect which side of walls we hit
    SOUTH,
    NORTH,
    EAST,
    WEST
};

struct Position {
    double x; 
    double y;
};

typedef struct Position position;

map create_map(char* world,int width,int height, double scale);

position get_player_pos(map m);

char* read_world(char* filename);
#endif