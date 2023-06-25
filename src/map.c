#include "../include/map.h"

map create_map(char* world,int width,int height, double scale){
    map m = malloc(sizeof(struct Map));
    m->world = world;
    m->width = width;
    m->height = height;
    m->scale = scale;
    return m;
}

position get_player_pos(map m){
    position player_pos = {.x = -1, .y = -1};
    for (int i = 0; i < m->height; i++){
        for (int j = 0; j < m->width; j++){
            if (m->world[i * m->width + j] == 'p'){
                player_pos.x = j * m->scale;
                player_pos.y = i * m->scale;
                return player_pos;
            }
        }
    }
    return player_pos;
}

char* read_world(char* filename,int* world_width,int* world_height){
    *world_width = 0;
    *world_height = 0;
    char* buffer;
    FILE* fp = fopen (filename ,"rb");
    fseek( fp , 0L , SEEK_END);
    long text_size = ftell(fp);
    rewind(fp);
    buffer = calloc( 1, text_size + 1 );
    fread( buffer , text_size, 1 , fp);
    fclose(fp);
    return buffer;
}