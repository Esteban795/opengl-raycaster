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

char* import_world(char path[],int* world_width,int* world_height){
    *world_width = 0;
    *world_height = 0;
	FILE* f = fopen(path, "rb");
	if (f == NULL) {
		printf("Error! Failed opening file %s\n", path);
		exit(1);
	}
	fseek(f, 0, SEEK_END);
	long len = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* data = malloc((len + 1) * sizeof(char));
	if (data == NULL) { //it happened once and I don't know why ? So I made an edge case
		printf("Error! Failed allocating memory.\n");
		exit(2);
	}
	int ch;
	int map_index = 0;
	int curr_row_len = 0, prev_row_len = 0;
	int first_row = true;
	while ((ch = fgetc(f)) != EOF) {
		switch (ch) {
            case '\n': //we're done reading a line
                if (first_row) {
                    first_row = false;
                    *world_width = curr_row_len;
                } else if (curr_row_len != prev_row_len) { //make sure each row is the same size
                    printf("Map rows have different sizes. Please use a rectangular bounded map.\n");
                    exit(3);
                }
                prev_row_len = curr_row_len;
                curr_row_len = 0;
                *world_height += 1;
                break;
            default:
                curr_row_len++;
                data[map_index++] = ch;
                break;
            }
        }
	*world_height += 1; // gets updated on newlines only, didnt increment in the very last line ending with EOF
	fclose(f);
	data[map_index] = '\0';
	return data;
}