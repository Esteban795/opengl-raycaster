SRCDIR = src
HEADDIR = include
LIBDIR = ./src

DEBUGFLAGS = -W -Wall -Wextra -Wvla -fsanitize=address
FLAGS = $(shell sdl2-config --cflags) -lSDL2 -lGL -lpthread -lglfw -lglut -lGLEW -lm
DEPENDENCIES = $(SRCDIR)/map.c $(SRCDIR)/ray.c $(SRCDIR)/shader.c  $(SRCDIR)/window.c 

build:
	gcc $(SRCDIR)/raycaster.c -o ./bin/raycaster $(DEPENDENCIES) $(FLAGS) 

run:
	./bin/raycaster

clean:
	rm ./bin/raycaster

debug:
	gcc $(SRCDIR)/raycaster.c -o ./bin/raycaster $(DEPENDENCIES) $(FLAGS) $(DEBUGFLAGS) 

all:
	make build
	make run

install:
	apt install libglew-dev libsdl2-dev freeglut3-dev