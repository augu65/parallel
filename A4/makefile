all: parallel serial

parallel: maze.c
	gcc -g maze.c -DPARALLEL -o mazep -fopenmp

serial: maze.c
	gcc -g maze.c -o maze -fopenmp