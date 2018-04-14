#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

SDL_Surface *load_surface(char path[1]);
SDL_Surface *create_color_surface(Uint32 width, Uint32 height, Uint32 r,
                                  Uint32 g, Uint32 b);

SDL_Surface *load_surface(char path[1])
{
    SDL_Surface *surface = SDL_LoadBMP(path);
    if (!surface) {
        printf("Screen surface could not be created, %s\n", SDL_GetError());
    }
    return surface;
}

SDL_Surface *create_color_surface(Uint32 width, Uint32 height, Uint32 r,
                                  Uint32 g, Uint32 b)
{
    SDL_Surface *surface =
        SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    if (!surface) {
        printf("Couldn't create RGB surface, %s\n", SDL_GetError());
        return NULL;
    }
    if (!SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r, g, b))) {
        printf("Couldn't fill surface, %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return NULL;
    }
    return surface;
}

int main(int argc, char *argv[argc + 1])
{
    SDL_Window *window = NULL;
    SDL_Surface *screen_surface = NULL;
    SDL_Surface *splash_image = NULL;
    //    SDL_Surface *white_surface = NULL;
    SDL_Surface *black_surface = NULL;
    SDL_Rect tile;
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

    splash_image = load_surface("hello_world.bmp");
    if (!splash_image) {
        status = EXIT_FAILURE;
        goto cleanup1;
    }

    black_surface = create_color_surface(100, 100, 255, 0, 0);
    if (!black_surface) {
        status = EXIT_FAILURE;
        goto cleanup2;
    }

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                }
            }
        }
        tile.x = 10;
        tile.y = 10;
        SDL_BlitSurface(splash_image, NULL, screen_surface, NULL);
        //        SDL_BlitSurface(white_surface, NULL, screen_surface, NULL);
        SDL_BlitSurface(black_surface, NULL, screen_surface, &tile);
        SDL_UpdateWindowSurface(window);
    }
// cleanup3:
//    SDL_FreeSurface(white_surface);
cleanup2:
    SDL_FreeSurface(black_surface);
cleanup1:
    SDL_FreeSurface(splash_image);
cleanup0:
    SDL_DestroyWindow(window);
    SDL_Quit();

    return status;
}
