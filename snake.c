#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int main(int argc, char *argv[argc + 1])
{
    SDL_Window *window = NULL;
    SDL_Surface *screen_surface = NULL;

    int status = EXIT_SUCCESS;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
	printf("SDL could not initialize, %s\n", SDL_GetError());
	return EXIT_FAILURE;
    }

    window = SDL_CreateWindow("SDL Test", SDL_WINDOWPOS_UNDEFINED,
			      SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
			      SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
	printf("Window could not be created, %s\n", SDL_GetError());
	return EXIT_FAILURE;
    }

    screen_surface = SDL_GetWindowSurface(window);
    if (!screen_surface) {
	printf("Screen surface could not be created, %s\n", SDL_GetError());
	status = EXIT_FAILURE;
	goto CLEANUP;
    }

    SDL_FillRect(screen_surface, NULL,
		 SDL_MapRGB(screen_surface->format, 0xff, 0xff, 0xff));
    SDL_UpdateWindowSurface(window);
    SDL_Delay(2000);

CLEANUP:
    SDL_DestroyWindow(window);
    SDL_Quit();

    return status;
}
