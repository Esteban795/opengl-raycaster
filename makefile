SRCDIR = src
HEADDIR = include
LIBDIR = ./src

CFLAGS = -W -Wall -Wextra -Wvla -fsanitize=address $(shell sdl2-config --cflags) -lSDL2 -lGL -lpthread -lXrandr -lglfw -ldl -lglut -lm

DEPENDENCIES = $(SRCDIR)/map.c

build:
	gcc $(SRCDIR)/raycaster.c -o ./bin/raycaster $(DEPENDENCIES) $(CFLAGS) 

run:
	./bin/raycaster $(FILENAME)

clean:
	rm ./bin/raycaster

all:
	make build
	make run FILENAME="./worlds/world1.txt";