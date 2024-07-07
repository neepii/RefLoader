#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>

SDL_Window * window;
SDL_Renderer * render;
SDL_Surface * flower;
SDL_Surface * surface;

const int height = 1500;
const int width  = 1500;


bool init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "ERROR: Init Failed: %s\n", SDL_GetError());
        return false;
    }

    int flags = IMG_INIT_PNG;
    if ( !( IMG_Init( flags ) & flags ) ) {
        fprintf(stderr, "ERROR: Cant Init an IMG. %s\n", SDL_GetError());
        return false;
    }
    window = SDL_CreateWindow("cross", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    if (window == NULL) {
        fprintf(stderr, "ERROR: Window is NULL. %s\n", SDL_GetError());
        return false;
    }
    
    
    surface = SDL_GetWindowSurface(window);

    
    return true;

};

bool load() {
    flower = IMG_Load("cross.png");
    if (!flower) {
        fprintf(stderr, "ERROR:%s", IMG_GetError());
        return false;
    }
    flower = SDL_ConvertSurface(flower, surface->format, NULL);
    flower = SDL_ConvertSurfaceFormat(flower,SDL_PIXELFORMAT_ARGB8888, 0);
    if (!flower) {
        fprintf(stderr, "ERROR: cant convert. %s", IMG_GetError());
        return false;
    }
    return true;
}

int main(int argc, char** argv) {
    
    if(!init()) {
        fprintf(stderr, "ERROR:%s", SDL_GetError());
        SDL_Quit();
    };
    if(!load()) {
        fprintf(stderr, "ERROR:%s", SDL_GetError());
        SDL_Quit();
    };

    bool loop = true;
    SDL_SetWindowAlwaysOnTop(window, SDL_TRUE);
    while(loop) {

        SDL_Rect rect;
        rect.h = 200;
        rect.w = 200;
        rect.x = (width/2) - (rect.w/2);
        rect.y = (height/2) - (rect.h/2);
        // SDL_BlitScaled(flower, NULL, surface, &rect);

        SDL_Event eve;
            while (SDL_PollEvent(&eve)) {
            switch (eve.type)
            {
            case SDL_QUIT:
                loop = SDL_FALSE;
                break;
            
            default:
                break;
            }
        }

        SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
        SDL_RenderClear(render);
        SDL_RenderPresent(render); //segfault(

        SDL_UpdateWindowSurface(window);
        SDL_RaiseWindow(window);
        SDL_Delay(10);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
