#include "../include/shader.h"

void shader_init(Shader* shader, const char* vertex_source, const char* fragment_source){
    *shader = malloc(sizeof(struct Shader));
    
    //creating vertex shader
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, NULL);
    
    glCompileShader(vertex_shader);


    //creating fragment shader
    unsigned int fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, NULL);
    glCompileShader(fragment_shader);

    (*shader)->program = glCreateProgram();
    //linking shaders to the program
    glAttachShader((*shader)->program, vertex_shader);
    glAttachShader((*shader)->program, fragment_shader);
    glLinkProgram((*shader)->program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void shader_destroy(Shader shader) {
    glDeleteProgram(shader->program);
}

void shader_use(Shader shader) {
    glUseProgram(shader->program);
}