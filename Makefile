
CC := gcc
FLAGS := `sdl2-config --cflags --libs`

all: main.c
	$(CC) main.c -o snoweth $(FLAGS)

rngtest: rng/rngtest.c
	$(CC) rng/rngtest.c -o rng/rngtest $(FLAGS)
