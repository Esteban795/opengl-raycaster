#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#define _USE_MATH_DEFINES
#include <math.h>

// Glew must come before opengl else it won't work.
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "../include/window.h"
#include "../include/shader.h"
#include "../include/map.h"
#include "../include/ray.h"

#define WIDTH 1024
#define HEIGHT 768
#define NUM_THREADS 4

const float quad_vertex_vata[] = { 
    // pos      // tex
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 

    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f
};

window win = NULL;
Shader shader = NULL;
map m;
position player_pos;
SDL_Event event;

const double world_scale = 10;
double player_angle = 0;
const double player_speed = 70;
const double mouse_sensitivity = 20;
const double fov = 100;
double half_fov;
double focus_to_image;
const unsigned int light_color = 0xFFFFFF;

/* const double max_fog_distance = 20;
const double min_fog_distance = 2;
const unsigned int fog_color = 0x87CEEB; */
char texture_data[WIDTH * HEIGHT * 3];
GLuint vao = -1;
GLuint screen_texture = -1;

bool quit = false;
bool keydown_z = false;
bool keydown_q = false;
bool keydown_s = false;
bool keydown_d = false;
bool keydown_left = false;
bool keydown_right = false;

int mouse_move_x = 0;

const char* vertex_shader_source = "#version 300 es\n"
    "precision highp float;\n"
    "layout (location = 0) in vec4 vertex;\n"  // <vec2 position, vec2 texCoords>
    "out vec2 TexCoords;\n"
    "void main() {\n"
        "TexCoords = vertex.zw;\n"
        "gl_Position = vec4(2.0f*vertex.x - 1.0f, 1.0f - 2.0f*vertex.y, 0.0, 1.0);\n"
    "}\0";

const char* fragment_shader_source = "#version 300 es\n"
    "precision highp float;\n"
    "in vec2 TexCoords;\n"
    "out vec4 color;\n"
    "uniform sampler2D screenTexture;\n"
    "void main() {\n"
        "color = vec4(texture(screenTexture, vec2(TexCoords.x, TexCoords.y)).rgb, 1.0f);\n"
        // "color = vec4(1.0f*TexCoords.x, 0.6f*TexCoords.y, 1.0f, 1.0f);\n"
    "}\0";

void create_screen_texture(void){
    glGenTextures(1, &screen_texture);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 

    glBindTexture(GL_TEXTURE_2D, 0); 
}

void create_quad_VAO() {

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // The verticies will never change so the buffer ID is not saved
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertex_vata),quad_vertex_vata, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // Free bound buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

void* render_thread(void* thread_num) {
    float thread_div = (float) WIDTH / NUM_THREADS;
    int thread_start = thread_div * *((int*)thread_num);
    int thread_end = thread_div * (*((int*)thread_num) + 1);

    for (int x = thread_start; x < thread_end; x++) {

        ray ray = cast_ray(m, player_pos, player_angle + atan((x-(WIDTH/2))/focus_to_image), player_angle);

        //ray.color = lerpColor(light_color, ray.color, sqrt(sin(ray.angle_of_incidence))); // Specular highlight

        int wall_height = (int) (( HEIGHT / (ray.depth)));
        for (int y = 0; y < HEIGHT; y++) {
            if (y > (HEIGHT - wall_height) / 2 && y < wall_height + (HEIGHT - wall_height) / 2) {
                texture_data[(y * WIDTH + x) * 3] = ray.color >> 16;
                texture_data[(y * WIDTH + x) * 3 + 1] = ray.color >> 8;
                texture_data[(y * WIDTH + x) * 3 + 2] = ray.color >> 0;
            }
        }
    }
    pthread_exit(0);
}

void render(pthread_t* threads,int* thread_args) {
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&(threads[i]), NULL, render_thread, (void*) (&(thread_args[i])));
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    glBindTexture(GL_TEXTURE_2D, screen_texture); 
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT,GL_RGB, GL_UNSIGNED_BYTE,texture_data);
    glClear(GL_COLOR_BUFFER_BIT);
    shader_use(shader);
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glUseProgram(0);
    window_swap(win);
    glBindTexture(GL_TEXTURE_2D, 0); 
}

void updatePlayer(uint64_t delta) {
    // Mouse movement
    player_angle += mouse_move_x / 1000.0 * mouse_sensitivity;
    // Arrow movement
    if (keydown_left != keydown_right) {
        float change = (keydown_left) ? -1 : 1;
        float arrow_speed = 3.5f;
        player_angle += (delta / 1000.0) * change * arrow_speed;
    }
    float x_fraction;
    float y_fraction;
    float mult = (delta/1000.0) * player_speed;
    if ((keydown_z != keydown_s)) {
        mult = keydown_s ? mult*-1 : mult;
        x_fraction = -sin(player_angle);
        y_fraction = cos(player_angle);
        player_pos.x += x_fraction * mult;
        player_pos.y += y_fraction * mult;
    }
    if ((keydown_q != keydown_d)) {
        mult = keydown_s ? -mult : mult;
        float turn_angle = keydown_d ? M_PI_2 : -M_PI_2;
        x_fraction = -sin(player_angle + turn_angle);
        y_fraction = cos(player_angle + turn_angle);
        player_pos.x += x_fraction *mult;
        player_pos.y += y_fraction * mult;
    }
}

void pollEvents() {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    glViewport(0, 0, event.window.data1, event.window.data2);
                }
                break;
            case SDL_MOUSEMOTION:
                mouse_move_x = event.motion.xrel;
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_z:
                        keydown_z = true;
                        break;
                    case SDLK_q:
                        keydown_q = true;
                        break;
                    case SDLK_s:
                        keydown_s = true;
                        break;
                    case SDLK_d:
                        keydown_d = true;
                        break;
                    case SDLK_LEFT:
                        keydown_left = true;
                        break;
                    case SDLK_RIGHT:
                        keydown_right = true;
                        break;
                }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym) {
                    case SDLK_z:
                        keydown_z = false;
                        break;
                    case SDLK_q:
                        keydown_q = false;
                        break;
                    case SDLK_s:
                        keydown_s = false;
                        break;
                    case SDLK_d:
                        keydown_d = false;
                        break;
                    case SDLK_LEFT:
                        keydown_left = false;
                        break;
                    case SDLK_RIGHT:
                        keydown_right = false;
                        break;
                }
                break;
            default:
                // Ignore other events
                break;
        }
    }
}

char world[] = 
    "rrrrrrrrrrrrrrrrrrrrrrrrrrrrrr"
    "b                 g          g"
    "b                 g          g"
    "b   p             g          g"
    "b                 g          g"
    "b                 g          g"
    "b                 g          g"
    "b     ggggggggggggg     #####g"
    "b                 g          g"
    "b  r              g          g"
    "b                 g          g"
    "b                            g"
    "b                            g"
    "b    gb           rrrrr    bbg"
    "b                 r          g"
    "b                 r          g"
    "b                 r          g"
    "b                 r          g"
    "b###################  ########";
const int world_width = 30;
const int world_height = 19;

int main(void){
    //int world_width,world_height;
    //char* world = read_world(argv[2]);
    win = window_create(WIDTH, HEIGHT);
    m = create_map(world, world_width, world_height, world_scale);
    player_pos = get_player_pos(m);
    
    // thread args
/*     pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    for (int it = 0; it < NUM_THREADS; it++) {
        thread_args[it] = it;
    }
 */
    if (!window_init(win)) {
        printf("Failed to initialize SDL\n");
        return 0;

    } else {

        printf("SDL initialized\n");

        shader_init(&shader, vertex_shader_source, fragment_shader_source);
        printf("les shaders sont ok");
/*         create_screen_texture();

        glClearColor(0.5, 0.2, 0.5, 1.0);
        create_quad_VAO(); */

        half_fov = 0.5 * (fov / 180.0f * M_PI);
        focus_to_image = (WIDTH / 2) / tan(half_fov);

        uint64_t last_frame = SDL_GetTicks64();
        uint64_t current_frame = SDL_GetTicks64();
        uint64_t delta_time = 0;
        while (!quit) {

            current_frame = SDL_GetTicks64();
            delta_time = current_frame - last_frame;
            last_frame = current_frame;
            // Poll for events
/*             pollEvents();
            updatePlayer(delta_time); */
            //render(threads,thread_args);
            mouse_move_x = 0;
        }
    }  

    glDeleteBuffers(1, &vao);
    glDeleteTextures(1, &screen_texture);

    window_destroy(win);
    shader_destroy(shader);

    return 0;
}