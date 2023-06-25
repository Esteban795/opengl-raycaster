#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

struct Shader {
    GLuint program;
};

typedef struct Shader* Shader;

void shader_init(Shader* shader, const char* vertex_source, const char* fragment_source);

void shader_destroy(Shader shader);

void shader_use(Shader shader);