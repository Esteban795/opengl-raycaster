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
        return M_PI_2 - (angle - M_PI - M_PI_2);
    case EAST:
        return M_PI_2 - (angle - M_PI_2);
    case NORTH:
        return M_PI_2 - angle;
    case SOUTH:
        return M_PI_2 - (angle - M_PI);
    default:
        return 0;
    }
}

enum Side get_side_hit(int x, int y, int prev_x,int prev_y){
    if (x > prev_x) return WEST;
    if (x < prev_x) return EAST;
    if (y > prev_y) return NORTH;
    if (y < prev_y) return SOUTH;
    return WEST;
}

double get_ray_depth(position player_pos,int map_x, int map_y, double dx, double dy, double scale, enum Side side){
    if (dx == 0){
        int add_height = dy < 0 ? 1 : 0; //when casting in negative directions, you need to account for the width of map cells
        return fabs((map_y + add_height) * scale - player_pos.y);
    }
    if (dy == 0){
        int add_width = dx < 0 ? 1 : 0;
        return fabs((map_x + add_width) * scale - player_pos.x);
    }
    double x,y;
    if (side == NORTH || side == SOUTH){
        double slope = dy/dx;
        int add_height = side == SOUTH ? 1 : 0;
        y = (map_y + add_height) * scale;
        x = y/slope + player_pos.x - player_pos.y/slope;
    } else {
        double slope = dx/dy;
        int add_width = side == EAST ? 1 : 0;
        x = (map_x + add_width) * scale;
        y = x/slope + player_pos.y - player_pos.x/slope;
    }
    return sqrt((player_pos.x - x) * (player_pos.x - x) + (player_pos.y - y) * (player_pos.y - y));
}

ray cast_ray(map m,position pos,double ray_angle,double player_angle){
    ray r;
    int map_x = (int) (pos.x / m->scale);
    int map_y = (int) (pos.y / m->scale);
    int prev_map_x,prev_map_y;
    double march_x = 0.0;
    double march_y = 0.0;
    double detail = 10;
    double dx = -sin(ray_angle)/detail;
    double dy = cos(ray_angle)/detail;
    char curr; //what's the letter

    do {
        march_x += dx;
        march_y += dy;
        prev_map_x = map_x;
        prev_map_y = map_y;
        map_x = (int) ((pos.x + march_x) / m->scale);
        map_y = (int) ((pos.y + march_y) / m->scale);
        if (map_x < 0 || map_x >= m->width || map_y < 0 || map_y > m->height){ // outside of the map
            r.depth = 10000000;
            r.color = 0x000000;
            return r;
        }
        curr = m->world[map_y * m->width + map_x];
    } while (curr == ' ' || curr == 'p');
    if ( map_x != prev_map_x && map_y != prev_map_y) {
        //weird case where the ray goes perfectly through a corner so ray hit isn't registered..
        //we just cast another ray with a very small different angle
        return cast_ray(m,pos,ray_angle - 0.001,player_angle);
    }
    enum Side side = get_side_hit(map_x,map_y,prev_map_x,prev_map_y);
    double depth = get_ray_depth(pos,map_x,map_y,dx,dy,m->scale,side) / m->scale;
    r.depth = fabs(cos(player_angle - ray_angle) / depth); //find distance to the plane of the player rather than to the player directly to avoid fisheye effect
    r.color = get_ray_color(curr);
    r.angle_of_incidence = get_incidence_angle(side,ray_angle);
    return r;
}