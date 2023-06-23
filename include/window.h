#ifndef WINDOW_H
#define WINDOW_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

struct Window {
    int width;
    int height;
    SDL_Window* window;
    SDL_GLContext context; //necessary to work with opengl and SDL2
};


typedef struct Window* window;

window window_create(int width, int height);

void window_destroy(window window);

bool window_init(window window);

void window_swap(window window);

#endif