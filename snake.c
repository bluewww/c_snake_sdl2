#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

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
void update_snake();
int alloc_snake(struct snake_part **, struct snake_part **);
void free_snake(struct snake_part *);

void render_world(struct world *, struct snake_part *, SDL_Surface *);
void update_all();

int alloc_snake(struct snake_part **head, struct snake_part **tail)
{
    if (!head || !tail)
        return -1;
    /* We use this to zero all struct entries */
    struct snake_part zero = {0};

    /* allocate both head and tail */
    *head = malloc(sizeof(struct snake_part));
    if (!head) {
        printf("Snake allocation failed, %s\n", strerror(errno));
        return -1;
    }

    *tail = malloc(sizeof(struct snake_part));
    if (!tail) {
        free(head);
        printf("Snake allocation failed, %s\n", strerror(errno));
        return -1;
    }
    **head = zero;
    **tail = zero;

    /* Put to initial position and connect */
    (*head)->x = 1;
    (*head)->y = 0;
    (*head)->prev = *tail;
    (*tail)->x = 0;
    (*tail)->y = 0;
    (*tail)->prev = NULL; /* not needed, actually */

    return 0;
}

void free_snake(struct snake_part *head)
{
    if (!head)
        return;
    struct snake_part **next_ptr = &head; /* pointer to the head pointer */
    struct snake_part *free_ptr;

    /*
     * backup copy of pointer since we are freeing the struct with the
     * original pointer
     */
    struct snake_part *saved_ptr;

    while (*next_ptr) {
        free_ptr = *next_ptr;
        saved_ptr = ((*next_ptr)->prev);
        next_ptr = &saved_ptr;
        free(free_ptr);
    }
}

// TODO set both const
void render_world(struct world *world, struct snake_part *snake_head,
                  SDL_Surface *surface)
{
    SDL_Rect rect;
    rect.w = (int)world->tile_size_x; /* unfortunate type chosen by SDL */
    rect.h = (int)world->tile_size_y;

    /* Render world tiles */
    for (uint32_t x = 0; x < world->world_size_x; x++) {
        for (uint32_t y = 0; y < world->world_size_y; y++) {
            rect.x = x * world->tile_size_x;
            rect.y = y * world->tile_size_y;
            SDL_BlitSurface(world->black_surface, NULL, surface, &rect);
        }
    }

    /* Render snake tiles*/
    struct snake_part **snake_ptr = &snake_head;

    while (*snake_ptr) {
        rect.x = (*snake_ptr)->x * world->tile_size_x;
        rect.y = (*snake_ptr)->y * world->tile_size_y;
        SDL_BlitSurface(world->white_surface, NULL, surface, &rect);
        snake_ptr = &((*snake_ptr)->prev);
    }
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

    /* Do not free this surface, per documentation */
    screen_surface = SDL_GetWindowSurface(window);
    if (!screen_surface) {
        printf("Screen surface could not be created, %s\n", SDL_GetError());
        status = EXIT_FAILURE;
        goto fail_screen;
    }

    splash_image = load_surface("hello_world.bmp");
    if (!splash_image) {
        status = EXIT_FAILURE;
        goto fail_splash;
    }

    /* Init world stuff */
    // TODO: finish the snake
    struct snake_part *snake_head = NULL;
    struct snake_part *snake_tail = NULL;
    if (alloc_snake(&snake_head, &snake_tail)) {
        status = EXIT_FAILURE;
        goto fail_snake;
    }

    struct world world;
    world.tile_size_x = 16;
    world.tile_size_y = 16;
    world.world_size_x = 40;
    world.world_size_y = 30;

    world.black_surface =
        create_color_surface(world.tile_size_x, world.tile_size_y, 0, 0, 0);
    if (!world.black_surface) {
        status = EXIT_FAILURE;
        goto fail_black;
    }

    world.white_surface = create_color_surface(
        world.tile_size_x, world.tile_size_y, 255, 255, 255);
    if (!world.white_surface) {
        status = EXIT_FAILURE;
        goto fail_white;
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
        /* SDL_BlitSurface(splash_image, NULL, screen_surface, NULL); */
        /* SDL_BlitSurface(world.white_surface, NULL, screen_surface, NULL); */
        /* SDL_BlitSurface(world.black_surface, NULL, screen_surface, &tile); */
        render_world(&world, snake_head, screen_surface);
        SDL_UpdateWindowSurface(window);
    }

    SDL_FreeSurface(world.white_surface);
    world.white_surface = NULL;
fail_white:
    SDL_FreeSurface(world.black_surface);
    world.black_surface = NULL;
fail_black:
    free_snake(snake_head);
fail_snake:
    SDL_FreeSurface(splash_image);
    splash_image = NULL;
fail_splash:
fail_screen:
    SDL_DestroyWindow(window);
    SDL_Quit();

    return status;
}
