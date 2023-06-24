#include "../include/map.h"


int main(int argc,char* argv[]){
    if (argc < 2) return EXIT_FAILURE;
    char* filename = argv[1];
    char* world = read_world(filename);
    printf("WOrld : \n%s",world);
    free(world);
}


//gcc raycaster.c -o raycaster