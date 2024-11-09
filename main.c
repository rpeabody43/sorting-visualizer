#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "algorithms.h"
#include "display.h"

#define WINDOW_MAX_W 1024
#define WINDOW_H 768

static struct {
    SDL_Window* sdl_window;
    SDL_Renderer* sdl_renderer;
    SDL_Texture* sdl_texture;

    SDL_Color* colors;

    uint32_t* pixel_buffer;
    int* last_rendered;

    int last_h1;
    int last_h2;
    int delay;

    int window_w;
    int window_h;
    int bar_width;
    float size_ratio;
} display;

void colors_init() {
    SDL_Color* colors = malloc(n * sizeof(SDL_Color));
    float inc = 360.0 / n;
    float h = 0;
    int x;
    for (int i = 0; i < n; i++, h += inc) {
        colors[i].r = 0;
        colors[i].g = 0;
        colors[i].b = 0;

        x = 255 * (1 - fabs(fmod(h / 60, 2) - 1));

        if (h < 60) {
            colors[i].r = 255;
            colors[i].g = x;
        } else if (h < 120) {
            colors[i].r = x;
            colors[i].g = 255;
        } else if (h < 180) {
            colors[i].g = 255;
            colors[i].b = x;
        } else if (h < 240) {
            colors[i].g = x;
            colors[i].b = 255;
        } else if (h < 300) {
            colors[i].r = x;
            colors[i].b = 255;
        } else {
            colors[i].r = 255;
            colors[i].b = x;
        }
    }

    display.colors = colors;
}

void display_init(int w, float ratio, int delay) {
    display.window_w = w;
    display.window_h = WINDOW_H;
    display.sdl_window = SDL_CreateWindow(
        "Sorting", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        display.window_w, display.window_h, 0
    );
    display.sdl_renderer = SDL_CreateRenderer(
        display.sdl_window, -1, SDL_RENDERER_ACCELERATED
    );
    display.sdl_texture = SDL_CreateTexture(
        display.sdl_renderer, SDL_PIXELFORMAT_RGBA8888, 
        SDL_TEXTUREACCESS_TARGET, display.window_w, display.window_h
    );

    display.pixel_buffer = malloc(
        display.window_w * display.window_h * sizeof(uint32_t)
    );
    display.last_rendered = malloc(n * sizeof(int));

    display.bar_width = display.window_w / n;
    display.size_ratio = ratio;

    display.last_h1 = -1;
    display.last_h2 = -1;
    display.delay = delay;

    colors_init();
}

void quit() {
    free(arr);
    free(display.last_rendered);
    free(display.pixel_buffer);
    free(display.colors);
    SDL_DestroyTexture(display.sdl_texture);
    SDL_DestroyRenderer(display.sdl_renderer);
    SDL_DestroyWindow(display.sdl_window);
    SDL_Quit();
    
    exit(0);
}

// Make draw take in indices to highlight different ways
void draw (int highlight1, int highlight2) {
    int update_l = 0;
    int update_r = 0;

    SDL_Rect rect;
    rect.x = display.window_w-display.bar_width;
    rect.w = display.bar_width;

    SDL_Color bar_color;

    SDL_SetRenderTarget(display.sdl_renderer, display.sdl_texture);
    for (int i = n-1; i >= 0; i--, rect.x -= display.bar_width) {
        if (
            arr[i] == display.last_rendered[i] 
            && i != display.last_h1 
            && i != display.last_h2
            && highlight1 > -1 
            && highlight2 > -1
            && i != highlight1 
            && i != highlight2
        ) {
            continue;
        }
        if (i == highlight1) {
            SDL_SetRenderDrawColor(display.sdl_renderer, 255, 255, 255, 255);
        } else if (i == highlight2) {
            SDL_SetRenderDrawColor(display.sdl_renderer, 128, 128, 128, 255);
        } else {
            bar_color = display.colors[arr[i]];
            SDL_SetRenderDrawColor(
                display.sdl_renderer, bar_color.r, bar_color.g, bar_color.b, 255
            );
        }
        // Draw actual bar
        rect.h = arr[i] * display.size_ratio;
        rect.y = display.window_h - rect.h;
        SDL_RenderFillRect(display.sdl_renderer, &rect);

        // Clear out top part
        rect.h = rect.y;
        rect.y = 0;
        SDL_SetRenderDrawColor(display.sdl_renderer, 18, 18, 18, 255);
        SDL_RenderFillRect(display.sdl_renderer, &rect);
        if (rect.x + rect.w > update_r) {
            update_r = rect.x + rect.w;
        }
        update_l = rect.x;
    }
    SDL_Rect update_rect = {
        .x = update_l,
        .w = update_r - update_l,
        .y = 0,
        .h = display.window_h
    };
    SDL_SetRenderTarget(display.sdl_renderer, NULL);
    SDL_RenderCopy(
        display.sdl_renderer, display.sdl_texture, &update_rect, &update_rect
    );
    SDL_RenderPresent(display.sdl_renderer);
    memcpy(display.last_rendered, arr, n * sizeof(int));
    display.last_h1 = highlight1;
    display.last_h2 = highlight2;
}

void disp (int h1, int h2, bool override_delay) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit();
        }
    }
    draw(h1, h2);
    if (display.delay && !override_delay) {
        SDL_Delay(display.delay);
    }
}

void(*init_with_args(int argc, char** argv))() {
    if (argc == 1) {
        printf("Usage: sorting {algorithm} {items=100} {delay=0}\n");
        printf("Run sorting --list for supported algorithms\n");
        exit(1);
    }
    
    int len = 100;
    if (argc > 2) {
        len = atoi(argv[2]);
        if (len > WINDOW_MAX_W) {
            printf("The maximum length that can be displayed is %d\n", WINDOW_MAX_W);
            return NULL;
        }
    } else {
        len = 100;
    }
    array_init(len);

    int delay = 0;
    if (argc > 3) {
        delay = atoi(argv[3]);
    }

    float size_ratio = (float) WINDOW_H / max_value;
    int real_width = WINDOW_MAX_W - (WINDOW_MAX_W % len);
    display_init(real_width, size_ratio, delay);

    if (strcmp(argv[1], "--list") == 0) {
        printf(
            "Available algorithms:\n"
            "bubble\n"
            "insertion\n"
            "selection\n"
            "merge\n"
            "quick\n"
            "quicklr\n"
            "heap\n"
            "counting\n"
            "radix\n"
            "bogo\n"
        );
    } else if (strcmp(argv[1], "bubble") == 0) {
        return bubble_sort;
    } else if (strcmp(argv[1], "insertion") == 0) {
        return insertion_sort;
    } else if (strcmp(argv[1], "selection") == 0) {
        return selection_sort;
    } else if (strcmp(argv[1], "merge") == 0) {
        return merge_sort;
    } else if (strcmp(argv[1], "quick") == 0) {
        return quick_sort;
    } else if (strcmp(argv[1], "quicklr") == 0) {
        return quick_sort_lr;
    } else if (strcmp(argv[1], "heap") == 0) {
        return heap_sort;
    } else if (strcmp(argv[1], "counting") == 0) {
        return counting_sort;
    } else if (strcmp(argv[1], "radix") == 0) {
        return radix_sort;
    } else if (strcmp(argv[1], "bogo") == 0) {
        printf("WARNING: THIS SORTING ALGORITHM IS A JOKE\n");
        printf("DON'T RUN IT ABOVE LENGTH 6 OR SO\n");
        return bogo_sort;
    } else {
        printf("Invalid sorting algorithm\n");
        exit(1);
    }
    return NULL;
}

int main(int argc, char** argv) {
    void (*sort)() = init_with_args(argc, argv);
    if (!sort) {
        exit(1);
    }

    if (SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL Failed To Initialize: %s", SDL_GetError());
        return 1;
    }

    shuffle();

    clock_t begin = clock();

    sort();

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    //print_arr();
    printf("%fs elapsed\n", time_spent);

    while (true) {
        disp(-1, -1, false);
    }
    
    quit();
}
