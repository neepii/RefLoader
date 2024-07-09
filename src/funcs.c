#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_syswm.h>
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

SDL_Window * window;
SDL_Renderer * render;
SDL_Surface * flower;
SDL_Surface * surface;

bool wMinimized = false;

const int wW = 200;
const int wH = 200;

bool MakeWindowTransparent(SDL_Window * window, COLORREF color);

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
    flower = IMG_Load("res/cross.png");
    if (!flower) {
        fprintf(stderr, "ERROR:%s", IMG_GetError());
        return false;
    }
    flower = SDL_ConvertSurfaceFormat(flower,SDL_PIXELFORMAT_ARGB8888, 0);
    if (!flower) {
        fprintf(stderr, "ERROR: cant convert. %s", IMG_GetError());
        return false;
    }
    return true;
}

void quit() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool MakeWindowTransparent(SDL_Window * window, COLORREF color) {

    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version);
    SDL_GetWindowWMInfo(window, &wminfo);

    HWND hwnd = wminfo.info.win.window;

    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);

    return SetLayeredWindowAttributes(hwnd, color, 0, LWA_COLORKEY);

};



void start() {
    
    if(!init()) {
        fprintf(stderr, "ERROR:%s", SDL_GetError());
        SDL_Quit();
    };
    if(!load()) {
        fprintf(stderr, "ERROR:%s", SDL_GetError());
        SDL_Quit();
    };

    bool loop = true;
    while(loop) {

        SDL_Rect rect;
        rect.h = 200;
        rect.w = 200;
        rect.x = (wW/2) - (rect.w/2);
        rect.y = (wH/2) - (rect.h/2);

        SDL_BlitScaled(flower, NULL, surface, &rect);

        SDL_Event eve;
        while (SDL_PollEvent(&eve)) {
            switch (eve.type)
            {
            case SDL_QUIT:
                loop = SDL_FALSE;
                break;

            case SDL_WINDOWEVENT_MINIMIZED:
                wMinimized = true;
                break;

            case SDL_WINDOWEVENT_MAXIMIZED:
                wMinimized = false;
                break;
        
            default:
                break;
            }
        }

        // if (wMinimized) {
        //     SDL_ShowWindow(window);
        //     SDL_RaiseWindow(window);
        //     SDL_SetWindowAlwaysOnTop(window, SDL_TRUE);
        // }

        // SDL_SetRenderDrawColor(render, 255, 255,255, 255);
        // SDL_RenderClear(render);
        // SDL_RenderPresent(render); //segfault(

        SDL_UpdateWindowSurface(window);
        SDL_Delay(10);
    }
}
