#include "../include/ray.h"

unsigned int get_ray_color(unsigned char symbol) {
    switch(symbol) {
        case 'r':
            return 0xFF2222;
        case 'g':
            return 0x22FF22;
        case 'b':
            return 0x2222FF;
        default:
            return 0x222222;
    }
}

double get_incidence_angle(enum Side side, double angle) {

    switch(side) {
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

enum Side get_side_hit(int x, int y, int prev_x, int prev_y) {

    if (x > prev_x) {
        return WEST;
    }
    if (x < prev_x) {
        return EAST;
    }
    if (y > prev_y) {
        return NORTH;
    }
    if (y < prev_y) {
        return SOUTH;
    }
}

double get_ray_depth(position player_position, int map_x, int map_y, double delta_x, double delta_y, double scale, enum Side side) {

    if (delta_x == 0) {

        int add_height = (delta_y < 0) ? 1 : 0; // When casting in negative directions, you need to account for the width of map cells
        return fabs((map_y + add_height)*scale - player_position.y);
    }

    if (delta_y == 0) {

        int add_width = (delta_x < 0) ? 1 : 0;
        return fabs((map_x + add_width)*scale - player_position.x);
    }

    if (side == NORTH || side == SOUTH) {

        double slope = delta_y/delta_x;
        int add_height = (side == SOUTH) ? 1 : 0;

        double y = (map_y + add_height) * scale;
        double x = y/slope + player_position.x - player_position.y/slope;

        return sqrt((player_position.x - x)*(player_position.x - x) + (player_position.y - y)*(player_position.y - y));
    }

    if (side == EAST || side == WEST) {

        double slope = delta_x/delta_y;
        int add_width = (side == EAST) ? 1 : 0;

        double x = (map_x + add_width) * scale;
        double y = x/slope + player_position.y - player_position.x/slope;

        return sqrt((player_position.x - x)*(player_position.x - x) + (player_position.y - y)*(player_position.y - y));
    }
}

ray cast_ray(map m, position position, double ray_angle, double player_angle) {

    ray r;

    int map_x = (int) (position.x/ m->scale);
    int map_y = (int) (position.y/ m->scale);

    int prev_map_x;
    int prev_map_y;

    double march_x = 0;
    double march_y = 0;

    double detail = 10; // This will determine how small the steps are when doing a raycast
    // Detail values too low will cause corners of cells be be lost due to resampling

    double delta_x = -sin(ray_angle)/detail;
    double delta_y = cos(ray_angle)/detail;

    char curr;

    do {
        march_x += delta_x;
        march_y += delta_y;

        prev_map_x = map_x;
        prev_map_y = map_y;

        map_x = (int) ((position.x+march_x)/m->scale);
        map_y = (int) ((position.y+march_y)/m->scale);

        if (map_x < 0 || map_x >= m->width || map_y < 0 || map_y >= m->height) {

            r.depth = 1000000;
            r.color = 0x000000;
            
            return r;
        }

        curr = m->world[map_y*m->width + map_x];

    } while (curr == ' ' || curr == 'P');

    if ((map_x != prev_map_x) && (map_y != prev_map_y)) {
        // There are cases where the ray passes perfectly through a corner and the side of the block that was hit cannot be known
        // In these cases, do the ray cast in a very slightly different direction
        return cast_ray(m, position, ray_angle - 0.0001, player_angle);
    } else {
        enum Side side = get_side_hit(map_x, map_y, prev_map_x, prev_map_y);
        double raw_depth = get_ray_depth(position, map_x, map_y, delta_x, delta_y, m->scale, side)/m->scale;
        r.depth = fabs(cos(player_angle - ray_angle)*raw_depth); // Find the distance to the plane of the player rather than to the player
        r.color = get_ray_color(curr);
        r.angle_of_incidence = get_incidence_angle(side, ray_angle);
    }

    return r;
}