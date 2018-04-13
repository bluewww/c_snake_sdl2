#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int main(int argc, char *argv[argc + 1])
{
    SDL_Window *window = NULL;
    SDL_Surface *screen_surface = NULL;
    SDL_Surface *splash_image = NULL;
    SDL_Event event;
    bool quit = false;

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
        goto cleanup0;
    }

    splash_image = SDL_LoadBMP("hello_world.bmp");
    if (!splash_image) {
        printf("Couldn't load image, %s\n", SDL_GetError());
        status = EXIT_FAILURE;
        goto cleanup1;
    }

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
        SDL_BlitSurface(splash_image, NULL, screen_surface, NULL);
        SDL_UpdateWindowSurface(window);
    }

cleanup1:
    SDL_FreeSurface(splash_image);
cleanup0:
    SDL_DestroyWindow(window);
    SDL_Quit();

    return status;
}
