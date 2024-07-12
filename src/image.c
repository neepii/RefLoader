#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_syswm.h>
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include "init.h"
#define HOTKEY1 1110

static SDL_Window * window;
static SDL_Renderer * render;
static SDL_Surface * crosshair_img;
static SDL_Surface * surface;


static int wW = 400;
static int wH = 400;

int XShift = 0;
int YShift = 0;

static bool MakeWindowTransparent(SDL_Window * window, COLORREF color);

static bool init() {

    window = SDL_CreateWindow("image", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, wW, wH, SDL_WINDOW_OPENGL);
    render = SDL_CreateRenderer(window, -1,SDL_RENDERER_SOFTWARE);
    
    if (window == NULL) {
        fprintf(stderr, "ERROR: Window is NULL. %s\n", SDL_GetError());
        return false;
    }
    
    
    surface = SDL_GetWindowSurface(window);

    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
    SDL_RenderClear(render);

    //MakeWindowTransparent(window, RGB(255,0,255));
    
    SDL_RenderPresent(render);

    SDL_SetWindowAlwaysOnTop(window, SDL_TRUE);

    return true;

};

static void MakeTray(SDL_Window * window) {

}

static bool load(char * path) {
    crosshair_img = IMG_Load(path);
    if (crosshair_img == NULL) {
        crosshair_img = IMG_Load("res/cross.png"); // default
    }
    if (!crosshair_img) {
        fprintf(stderr, "ERROR:%s", IMG_GetError());
        return false;
    }
    crosshair_img = SDL_ConvertSurfaceFormat(crosshair_img,SDL_PIXELFORMAT_ARGB8888, 0);
    if (!crosshair_img) {
        fprintf(stderr, "ERROR: cant convert. %s", IMG_GetError());
        return false;
    }

    wW = crosshair_img->w;
    wH = crosshair_img->h;

    return true;
}


static bool MakeWindowTransparent(SDL_Window * window, COLORREF color) { //winapi is a cool dude

    HWND hwnd = getHWND(window);

    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);

    return SetLayeredWindowAttributes(hwnd, color, 0, LWA_COLORKEY);

};



void CH_InitImage(char * path) {

    HWND hwnd = getHWND(window);
    MSG msg = {0};
    if (!RegisterHotKey(NULL, HOTKEY1, MOD_CONTROL | MOD_NOREPEAT | MOD_ALT, 0x52)) {
        fprintf(stderr, "ERROR:%s", SDL_GetError());
        CH_Quit();
    }
    
    if (!load(path)) {
        fprintf(stderr, "ERROR:%s", SDL_GetError());
        CH_Quit();
    };
    if (!init()) {
        fprintf(stderr, "ERROR:%s", SDL_GetError());
        CH_Quit();
    };

    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
    
    SDL_Rect rect;
    rect.h = 200;
    rect.w = 200;
    rect.x = (wW/2) - (rect.w/2) + XShift;
    rect.y = (wH/2) - (rect.h/2) + YShift;

    bool loop = true;
    while(loop) {
        SDL_BlitScaled(crosshair_img, NULL, surface, &rect);

        SDL_Event eve;
        while (SDL_PollEvent(&eve)) {
            GetMessage(&msg, hwnd, 0,0);
            switch (eve.type)
            {
            case SDL_QUIT:
                loop = false;
                break;
            case SDL_SYSWMEVENT:
                if (msg.message == WM_HOTKEY && LOWORD(msg.wParam) == HOTKEY1) {
                    loop = false;
                }
                break;
        
            default:
                break;
            }
            
        };



        SDL_UpdateWindowSurface(window);
        SDL_Delay(10);
    }
    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
}
