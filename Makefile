FLAGS=-O0 -g

all: puzzle.c
	gcc --std=c11 $(FLAGS) -o puzzle puzzle.c
