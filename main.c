#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

#define WINDOW_MAX_W 1024
#define WINDOW_H 768

static struct {
    SDL_Window* sdl_window;
    SDL_Renderer* sdl_renderer;

    int window_w;
    int window_h;
    int bar_width;
    float size_ratio;
} display;

static int* arr;
static int* last_rendered;
static int n;

void display_init(int w, float ratio) {
    display.window_w = w;
    display.window_h = WINDOW_H;
    display.sdl_window = SDL_CreateWindow(
        "Sorting", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        display.window_w, display.window_h, 0
    );
    display.sdl_renderer = SDL_CreateRenderer(
        display.sdl_window, -1, SDL_RENDERER_ACCELERATED
    );
    display.bar_width = display.window_w / n;
    display.size_ratio = ratio;
}

void display_cleanup() {
    SDL_DestroyRenderer(display.sdl_renderer);
    SDL_DestroyWindow(display.sdl_window);
}

void quit() {
    free(arr);
    display_cleanup();
    SDL_Quit();
    
    exit(0);
}

// Make draw take in indices to highlight different ways
void draw () {
    SDL_Rect rect;
    rect.x = display.window_w-display.bar_width;
    rect.w = display.bar_width;

    // Look into this to make draw faster
    // https://stackoverflow.com/questions/62379457/how-to-draw-a-grid-of-pixels-fast-with-sdl2
    for (int i = n-1; i >= 0; i--, rect.x -= display.bar_width) {
        if (arr[i] == last_rendered[i]) {
            continue;
        }
        // Draw actual bar
        rect.h = arr[i] * display.size_ratio;
        rect.y = display.window_h - rect.h;
        SDL_SetRenderDrawColor(display.sdl_renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(display.sdl_renderer, &rect);

        // Clear out top part
        rect.h = rect.y;
        rect.y = 0;
        SDL_SetRenderDrawColor(display.sdl_renderer, 18, 18, 18, 255);
        SDL_RenderFillRect(display.sdl_renderer, &rect);
    }
    SDL_RenderPresent(display.sdl_renderer);
    memcpy(last_rendered, arr, n * sizeof(int));
}

void disp (int delay) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit();
        }
    }
    draw();
    if (delay) {
        SDL_Delay(delay);
    }
}

void fill (int len, int max) {
    float increment = (float) max / len;
    int val = 0;
    for (int i = len-1; i >= 0; i--) {
        arr[i] = val += increment;
    }
    n = len;
}

void bubble () {
    int temp;
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                temp = arr[j+1];
                arr[j+1] = arr[j];
                arr[j] = temp;
            }
        }
        disp(0);
    }
}

// Fisher-Yates Shuffle
void shuffle () {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int usec = tv.tv_usec;
    srand48(usec);

    int temp;
    for (int i = 0; i < n - 1; i++) {
        size_t j = (unsigned int) (drand48()*(i+1));
        temp = arr[j];
        arr[j] = arr[i];
        arr[i] = temp;
        disp(0);
    }
}

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL Failed To Initialize: %s", SDL_GetError());
        return 1;
    }

    int len = 1000;
    int max = len;
    arr = malloc(sizeof(int) * len);
    last_rendered = malloc(sizeof(int) * len);

    fill(len, max);

    float size_ratio = (float) WINDOW_H / max;
    int real_width = WINDOW_MAX_W - (WINDOW_MAX_W % len);

    display_init(real_width, size_ratio);

    shuffle();

    clock_t begin = clock();

    bubble();

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%fs elapsed\n", time_spent);

    while (true) {
        disp(0);
    }
    
    quit();
}
