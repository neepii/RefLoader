#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_syswm.h>
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include "init.h"

static SDL_Window * window;
static SDL_Renderer * render;
static SDL_Surface * crosshair_img;
static SDL_Surface * surface;


static const int wW = 200;
static const int wH = 200;

int XShift = 0;
int YShift = 23;

static bool MakeWindowTransparent(SDL_Window * window, COLORREF color);

bool initcrosshair() {

    window = SDL_CreateWindow("cross", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, wW, wH, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
    render = SDL_CreateRenderer(window, -1,SDL_RENDERER_SOFTWARE);
    
    if (window == NULL) {
        fprintf(stderr, "ERROR: Window is NULL. %s\n", SDL_GetError());
        return false;
    }
    
    
    surface = SDL_GetWindowSurface(window);

    SDL_SetRenderDrawColor(render, 255, 0, 255, 255);
    SDL_RenderClear(render);

    MakeWindowTransparent(window, RGB(255,0,255));
    
    SDL_RenderPresent(render);

    SDL_SetWindowAlwaysOnTop(window, SDL_TRUE);

    return true;

};

bool load() {
    crosshair_img = IMG_Load("res/cross.png");
    if (!crosshair_img) {
        fprintf(stderr, "ERROR:%s", IMG_GetError());
        return false;
    }
    crosshair_img = SDL_ConvertSurfaceFormat(crosshair_img,SDL_PIXELFORMAT_ARGB8888, 0);
    if (!crosshair_img) {
        fprintf(stderr, "ERROR: cant convert. %s", IMG_GetError());
        return false;
    }
    return true;
}


static bool MakeWindowTransparent(SDL_Window * window, COLORREF color) { //winapi is a cool dude

    HWND hwnd = getHWND(window);

    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);

    return SetLayeredWindowAttributes(hwnd, color, 0, LWA_COLORKEY);

};



void start() {
    
    if(!initcrosshair()) {
        fprintf(stderr, "ERROR:%s", SDL_GetError());
        CH_Quit();
    };
    if(!load()) {
        fprintf(stderr, "ERROR:%s", SDL_GetError());
        CH_Quit();
    };

    bool loop = true;
    while(loop) {

        SDL_Rect rect;
        rect.h = 200;
        rect.w = 200;
        rect.x = (wW/2) - (rect.w/2) + XShift;
        rect.y = (wH/2) - (rect.h/2) + YShift;

        SDL_BlitScaled(crosshair_img, NULL, surface, &rect);

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


        SDL_UpdateWindowSurface(window);
        SDL_Delay(10);
    }
    SDL_DestroyWindow(window);
}
