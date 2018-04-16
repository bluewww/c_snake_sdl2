#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include <SDL2/SDL.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

struct world {
    uint32_t tile_size_x;  // 16;
    uint32_t tile_size_y;  // 16;
    uint32_t world_size_x; // 40;
    uint32_t world_size_y; // 30;
    SDL_Surface *black_surface;
    SDL_Surface *white_surface;
};

struct snake_part {
    struct snake_part *prev;
    uint32_t x;
    uint32_t y;
};

SDL_Surface *load_surface(const char path[1]);
SDL_Surface *create_color_surface(Uint32 width, Uint32 height, Uint32 r,
                                  Uint32 g, Uint32 b);
// TODO: snake queue
// TODO: food spawn logic
// TODO: struct world
void render_world(struct world *, struct snake_part *, SDL_Surface *);
void update_all();

void render_world(struct world *world, struct snake_part *snake,
                  SDL_Surface *surface)
{
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = (int)world->tile_size_x;
    rect.h = (int)world->tile_size_y;

    /* Render world */
    for (uint32_t x = 0; x < world->world_size_x; x++) {
        for (uint32_t y = 0; y < world->world_size_y; y++) {
            rect.x = x * world->world_size_x;
            rect.y = y * world->world_size_y;
            SDL_BlitSurface(world->white_surface, NULL, surface, &rect);
        }
    }
    //TODO: render snake
}
SDL_Surface *load_surface(const char path[1])
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
    if (SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r, g, b))) {
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
    SDL_Surface *white_surface = NULL;
    SDL_Surface *black_surface = NULL;
    SDL_Rect tile;
    SDL_Event event;
    bool quit = false;

    int status = EXIT_SUCCESS;

    /* Init SDL stuff */
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

    /* Init world stuff */
    // TODO: finish the snake
    struct snake_part snake;
    struct world world;
    world.tile_size_x = 16;
    world.tile_size_y = 16;
    world.world_size_x = 40;
    world.world_size_y = 30;

    world.black_surface = create_color_surface(100, 100, 0, 0, 0);
    if (!black_surface) {
        status = EXIT_FAILURE;
        goto cleanup2;
    }

    world.white_surface = create_color_surface(100, 100, 255, 255, 255);
    if (!white_surface) {
        status = EXIT_FAILURE;
        goto cleanup3;
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
        /* SDL_BlitSurface(splash_image, NULL, screen_surface, NULL); */
        /* SDL_BlitSurface(world.white_surface, NULL, screen_surface, NULL); */
        /* SDL_BlitSurface(world.black_surface, NULL, screen_surface, &tile); */
	render_world(&world, &snake, screen_surface);
        SDL_UpdateWindowSurface(window);
    }

cleanup3:
    SDL_FreeSurface(world.white_surface);
    world.white_surface = NULL;
cleanup2:
    SDL_FreeSurface(world.black_surface);
    world.black_surface = NULL;
cleanup1:
    SDL_FreeSurface(splash_image);
    splash_image = NULL;
cleanup0:
    SDL_DestroyWindow(window);
    SDL_Quit();

    return status;
}
