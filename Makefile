
CC := gcc
FLAGS := `sdl2-config --cflags --libs`

all: main.c
	$(CC) main.c -o cbsnow $(FLAGS)

rngtest: rng/rngtest.c
	$(CC) rng/rngtest.c -o rng/rngtest $(FLAGS)

#Create the cbsnownds folder and copy all the necessary files into it.
cbsnownds: d.h devents.h dplatform.h main.c platform/ndsd.h specialndsMakefile assets/font.h assets/drwslib.h
	-mkdir cbsnownds
	-mkdir cbsnownds/source
	-mkdir cbsnownds/source/platform
	-mkdir cbsnownds/source/assets
	cp -f d.h cbsnownds/source/d.h
	cp -f devents.h cbsnownds/source/devents.h
	cp -f dplatform.h cbsnownds/source/dplatform.h
	cp -f main.c cbsnownds/source/main.c
	cp -f platform/ndsd.h cbsnownds/source/platform/ndsd.h
	cp -f specialndsMakefile cbsnownds/Makefile
	cp -f assets/font.h cbsnownds/source/assets/font.h
	cp -f assets/drwslib.h cbsnownds/source/assets/drwslib.h
	make -C cbsnownds
