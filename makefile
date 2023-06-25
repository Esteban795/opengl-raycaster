SRCDIR = src
HEADDIR = include
LIBDIR = ./src

CFLAGS = -W -Wall -Wextra -Wvla -fsanitize=address $(shell sdl2-config --cflags) -lSDL2 -lGL -lpthread -lglfw -lglut -lGLEW -lm

DEPENDENCIES = $(SRCDIR)/map.c $(SRCDIR)/ray.c $(SRCDIR)/shader.c  $(SRCDIR)/window.c 

build:
	gcc $(SRCDIR)/raycaster.c -o ./bin/raycaster $(DEPENDENCIES) $(CFLAGS) 

run:
	./bin/raycaster

clean:
	rm ./bin/raycaster

all:
	make build
	make run