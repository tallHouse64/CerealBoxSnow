
CC := gcc
FLAGS := `sdl2-config --cflags --libs`

all:
	$(CC) main.c -o snowsim $(FLAGS)
