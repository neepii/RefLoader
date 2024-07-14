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
static SDL_Texture * imgtexture;
static SDL_Surface * imgsurface;
static SDL_Surface * surface;

int dragSide = 35;
static int wW;
static int wH;

POINT cursorpos;


static void UpdateImage() {
    SDL_SetRenderDrawColor(render,219,233,244,255);
    SDL_RenderClear(render);

    SDL_UpdateWindowSurface(window);
    
    int w,h;
    SDL_GetWindowSize(window, &w, &h);
    SDL_Rect winrect = {0, 0, w, h};

    SDL_Rect rect;
    rect.h = imgsurface->h;
    rect.w = imgsurface->w;
    rect.x = (winrect.w/2) - (rect.w/2);
    rect.y = (winrect.h/2) - (rect.h/2);
    imgtexture = SDL_CreateTextureFromSurface(render, imgsurface);
    SDL_RenderCopy(render, imgtexture, NULL, &rect);


    SDL_SetRenderDrawColor(render,255,255,255,255);
    SDL_Rect dragRect={winrect.w-dragSide,winrect.h-dragSide,dragSide, dragSide};
    SDL_RenderFillRect(render,&dragRect);

    SDL_DestroyTexture(imgtexture);    
    SDL_RenderPresent(render);
}

static bool init() {
    window = SDL_CreateWindow("image", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, wW, wH, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
    
    if (window == NULL) {
        fprintf(stderr, "ERROR: Window is NULL. %s\n", SDL_GetError());
        return false;
    }
    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (render == NULL) {
        fprintf(stderr, "ERROR: Renderer is NULL. %s\n", SDL_GetError());
        return false;
    }
    surface = SDL_GetWindowSurface(window);

    SDL_SetWindowAlwaysOnTop(window, SDL_TRUE);
    UpdateImage();

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


void CH_InitImage(char * path) {

    // HWND hwnd = getHWND(window);
    // MSG msg = {0};
    
    if (!load(path)) {
        fprintf(stderr, "ERROR:%s", SDL_GetError());
        CH_Quit();
    };
    if (!init()) {
        fprintf(stderr, "ERROR:%s", SDL_GetError());
        CH_Quit();
    };
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
    
    bool loop = true;
    bool holding = false;
    bool sizing = false;
    RECT winrect;

    while(loop) {
        GetWindowRect(getHWND(window), &winrect);
        SDL_Event eve;
        while (SDL_PollEvent(&eve)) {
            // GetMessage(&msg, hwnd, 0,0);
            switch (eve.type)
            {
            case SDL_QUIT:
                loop = false;
                break;
            // case SDL_SYSWMEVENT:
            //     break;
            case SDL_KEYDOWN:
                if (eve.key.keysym.sym == SDLK_q && KMOD_CTRL) {
                    loop = false;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                GetCursorPos(&cursorpos);

                if (cursorpos.x > (winrect.right-dragSide) &&
                    cursorpos.y > (winrect.bottom-dragSide)) {
                    sizing = true;
                } else if (eve.button.button == SDL_BUTTON_RIGHT) {
                    holding = true;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                holding = false;
                sizing = false;
                break;
        
            default:
                break;
            }
            
        };

        if (holding) {
            POINT temp = cursorpos;
            GetCursorPos(&cursorpos);

            int deltax = (int)(cursorpos.x - temp.x);
            int deltay = (int)(cursorpos.y - temp.y);

            int winx, winy;
            SDL_GetWindowPosition(window, &winx, &winy);  
            SDL_SetWindowPosition(window, winx + deltax, winy + deltay);          
        } else if (sizing) {
            POINT temp = cursorpos;
            GetCursorPos(&cursorpos);

            int deltax = (int)(cursorpos.x - temp.x);
            int deltay = (int)(cursorpos.y - temp.y);

            int winx, winy;
            SDL_GetWindowSize(window, &winx, &winy);
            SDL_SetWindowSize(window, winx + deltax, winy + deltay);
            UpdateImage();
        }

        SDL_Delay(10);
    }
    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
}
