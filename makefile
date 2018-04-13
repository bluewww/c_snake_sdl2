compile1: snake.c
	gcc-7 --std=c11 -Wall -o snake snake.c -lSDL2 -I.
