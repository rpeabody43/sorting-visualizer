#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "algorithms.h"
#include "display.h"

#define WINDOW_MAX_W 1024
#define WINDOW_H 768

static struct {
    SDL_Window* sdl_window;
    SDL_Renderer* sdl_renderer;
    SDL_Texture* sdl_texture;

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
        SDL_TEXTUREACCESS_STREAMING, display.window_w, display.window_h
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
}

void quit() {
    free(arr);
    free(display.last_rendered);
    free(display.pixel_buffer);
    SDL_DestroyTexture(display.sdl_texture);
    SDL_DestroyRenderer(display.sdl_renderer);
    SDL_DestroyWindow(display.sdl_window);
    SDL_Quit();
    
    exit(0);
}

// Make draw take in indices to highlight different ways
void draw (int highlight1, int highlight2) {
    const uint32_t standard_bar = 0xFFFFFFFF;
    const uint32_t color1 = 0x00FF00FF;
    const uint32_t color2 = 0x0000FFFF;
    const uint32_t background_color = 0x181818FF;
    uint32_t bar_color;

    int start_x, current_pos, height;
    uint32_t draw_color;

    int update_l = 0;
    int update_r = 0;

    start_x = display.window_w - display.bar_width;
    for (int i = n-1; i >= 0; i--, start_x -= display.bar_width) {
        if (
            arr[i] == display.last_rendered[i] 
            && i != display.last_h1 
            && i != display.last_h2
        ) {
            continue;
        }
        if (i == highlight1) {
            bar_color = color1;
        } else if (i == highlight2) {
            bar_color = color2;
        } else {
            bar_color = standard_bar;
        }
        height = arr[i] * display.size_ratio;

        // I think using a surface would make this faster since they're
        // more built for per-pixel software rendering
        int x, y;
        for (x = start_x; x < start_x + display.bar_width; x++) {
            draw_color = background_color;
            current_pos = x;
            for (y = 0; y < display.window_h; y++) {
                display.pixel_buffer[current_pos] = draw_color;
                current_pos += display.window_w;
                if (y >= display.window_h - height) {
                    draw_color = bar_color;
                }
            }
        }
        if (x > update_r) {
            update_r = x;
        }
        update_l = start_x;
    }
    // if this was never reassigned then nothing new needs to be rendered
    if (!update_r) { return; }
    SDL_Rect update_rect = {
        .x = update_l,
        .w = update_r - update_l,
        .y = 0,
        .h = display.window_h
    };
    SDL_UpdateTexture(
        display.sdl_texture, NULL, 
        display.pixel_buffer, display.window_w * sizeof(uint32_t)
    );
    SDL_RenderCopy(
        display.sdl_renderer, display.sdl_texture, &update_rect, &update_rect
    );
    SDL_RenderPresent(display.sdl_renderer);
    memcpy(display.last_rendered, arr, n * sizeof(int));
    display.last_h1 = highlight1;
    display.last_h2 = highlight2;
}

void disp (int h1, int h2) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit();
        }
    }
    draw(h1, h2);
    if (display.delay) {
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
    array_init(len, len);

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
        );
    } else if (strcmp(argv[1], "bubble") == 0) {
        return bubble_sort;
    } else if (strcmp(argv[1], "insertion") == 0) {
        return insertion_sort;
    } else if (strcmp(argv[1], "selection") == 0) {
        return selection_sort;
    } else if (strcmp(argv[1], "merge") == 0) {
        return merge_sort;
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
        disp(-1, -1);
    }
    
    quit();
}
