compile1: snake.c
	gcc-7 --std=c11 -ggdb -O0 -Wall -o snake snake.c -lSDL2 -I.
