FLAGS=-O3

all: puzzle.c
	gcc --std=c11 $(FLAGS) -o puzzle puzzle.c
