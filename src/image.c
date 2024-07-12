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
static SDL_Surface * imgsurface;
static SDL_Surface * surface;


static int wW;
static int wH;


static bool MakeWindowTransparent(SDL_Window * window, COLORREF color);

static void UpdateImage() {
    SDL_Rect rect;
    rect.h = imgsurface->h;
    rect.w = imgsurface->w;
    rect.x = (wW/2) - (rect.w/2);
    rect.y = (wH/2) - (rect.h/2);
    SDL_BlitScaled(imgsurface, NULL, surface, &rect);
}

static void UpdateImageOnResize() {
    SDL_SetRenderDrawColor(render,255,255,255,255);
    SDL_RenderClear(render);


    SDL_RenderPresent(render);
}

static bool init() {
    
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);

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


static bool load(char * path) {
    imgsurface = IMG_Load(path);
    if (imgsurface == NULL) {
        imgsurface = IMG_Load("res/cross.png"); // default
    }
    if (!imgsurface) {
        fprintf(stderr, "ERROR:%s", IMG_GetError());
        return false;
    }
    imgsurface = SDL_ConvertSurfaceFormat(imgsurface,SDL_PIXELFORMAT_ARGB8888, 0);
    if (!imgsurface) {
        fprintf(stderr, "ERROR: cant convert. %s", IMG_GetError());
        return false;
    }

    wW = imgsurface->w;
    wH = imgsurface->h;

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
    
    UpdateImage();
    bool loop = true;
    bool holding = false;
    while(loop) {


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
            // case SDL_MOUSEBUTTONDOWN:
            //     holding = true;
            //     break;
            // case SDL_MOUSEBUTTONUP:
            //     holding = false;
            //     break;
            case SDL_WINDOWEVENT:
                if (eve.window.event == SDL_WINDOWEVENT_RESIZED) {
                    UpdateImageOnResize();
                }

                break;
        
            default:
                break;
            }
            
        };
        
        // if(holding){
        // }



        SDL_UpdateWindowSurface(window);
        SDL_Delay(10);
    }
    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
}
