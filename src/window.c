#include "../include/window.h"


window window_create(int width, int height) {
    window win = malloc(sizeof(struct Window));
    win->height = height;
    win->width = width;
    return win;
}

void window_destroy(window win) {
	SDL_Destroywindow(win->window);
	SDL_Quit();
}

//creates an actual SDL window and a context for OpenGL to work with (through SDL2 context handling)
bool window_init(window win) {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		printf("SDL Error (initialization) : %s\n", SDL_GetError());
		return false;
	}

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //double buffering for better performance
    win->window = SDL_Createwindow("OpenGL Raycaster made by Esteban795 on Github", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, win->width, win->height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if(win->window == NULL) {
        printf("SDL error (window initialization) : %s\n", SDL_GetError());
        return false;
    }
    win->context = SDL_GL_CreateContext(win->window);
    if(win->context == NULL) {
        printf("SDL error (couldn't create OpenGL context) %s\n", SDL_GetError());
        return false;
    }
    // Use Vsync
    if( SDL_GL_SetSwapInterval( 2 ) < 0 ) {
        printf("SDL error (VSYNC not enabled) : %s\n", SDL_GetError());
    }
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void windowSwap(window win) {
    SDL_GL_Swapwindow(win->window);
}