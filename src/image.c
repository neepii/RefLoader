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
SDL_Rect rect;

double imgRatio;

int dragSide = 35;
POINT cursorpos;   


typedef enum IMGSTATE{
    NO,
    HOLD_WIN,
    HOLD_NW,
    HOLD_NE,
    HOLD_SW,
    HOLD_SE,
    HOLD_ALT
} imgstate;


static void ChangeCursor(LPCSTR cursorname)  {
    SetCursor(LoadCursor(NULL, cursorname));
}

static void UpdateImage() {
    SDL_SetRenderDrawColor(render,219,233,244,255);
    SDL_RenderClear(render);

    SDL_UpdateWindowSurface(window);
    
    int w,h;
    SDL_GetWindowSize(window, &w, &h);
    SDL_Rect winrect = {0, 0, w, h};

    // SDL_Rect rect;
    // rect.h = imgsurface->h;
    // rect.w = imgsurface->w;
    rect.x = (winrect.w/2) - (rect.w/2);
    rect.y = (winrect.h/2) - (rect.h/2);
    imgtexture = SDL_CreateTextureFromSurface(render, imgsurface);
    SDL_RenderCopy(render, imgtexture, NULL, &rect);


    SDL_SetRenderDrawColor(render,255,255,255,255);
    SDL_Rect dragRect0 ={0,0, dragSide, dragSide};
    SDL_Rect dragRect1 ={winrect.w-dragSide, 0, dragSide, dragSide};
    SDL_Rect dragRect2 ={0, winrect.h - dragSide, dragSide, dragSide};
    SDL_Rect dragRect3={winrect.w - dragSide,winrect.h-dragSide,dragSide, dragSide};
    SDL_Rect dragRects[4]={dragRect0,dragRect1,dragRect2,dragRect3};
    SDL_RenderFillRects(render,dragRects,4);

    SDL_DestroyTexture(imgtexture);    
    SDL_RenderPresent(render);
}

static bool init() {
    window = SDL_CreateWindow("image", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, imgsurface->w, imgsurface->h, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
    
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

    imgRatio = (double)imgsurface->w / (double)imgsurface->h;

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
    
    imgstate st;
    RECT winrect;
    rect.h = imgsurface->h;
    rect.w = imgsurface->w;
    bool loop = true;                          

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
                if (eve.key.keysym.sym == SDLK_q && eve.key.keysym.mod & KMOD_CTRL) {
                    loop = false;
                }
                else if (eve.key.keysym.sym == SDLK_r && eve.key.keysym.mod & KMOD_CTRL) {
                    load(path);
                    rect.h = imgsurface->h;
                    rect.w = imgsurface->w;     
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                GetCursorPos(&cursorpos);
                if (cursorpos.x > (winrect.right-dragSide) &&
                    cursorpos.y > (winrect.bottom-dragSide)) {
                    st = HOLD_SE;
                    ChangeCursor(IDC_SIZENWSE);
                }
                else if (cursorpos.x < (winrect.left+dragSide) &&
                         cursorpos.y > (winrect.bottom-dragSide)) {
                    st = HOLD_SW;
                    ChangeCursor(IDC_SIZENESW);
                }
                else if (cursorpos.x > (winrect.right-dragSide) &&
                         cursorpos.y < (winrect.top+dragSide)) {
                    st = HOLD_NE;
                    ChangeCursor(IDC_SIZENESW);
                }
                else if (cursorpos.x < (winrect.left+dragSide) &&
                         cursorpos.y < (winrect.top+dragSide)) {
                    st = HOLD_NW;
                    ChangeCursor(IDC_SIZENWSE);
                }
                else if((eve.key.keysym.mod & KMOD_LALT) &&
                        (eve.button.button == SDL_BUTTON_LEFT)) {
                    st = HOLD_ALT;
                    ChangeCursor(IDC_SIZEWE);
                }
                else if (eve.button.button == SDL_BUTTON_RIGHT) {
                    st = HOLD_WIN;
                    ChangeCursor(IDC_SIZEALL);
                }
                
                break;
            case SDL_MOUSEBUTTONUP:
                st = NO;
                break;
        
            default:
                break;
            }
            
    };
        POINT temp = cursorpos;
        GetCursorPos(&cursorpos);

        int deltax = (int)(cursorpos.x - temp.x);
        int deltay = (int)(cursorpos.y - temp.y);

        int winx, winy;
        int winw, winh;
        SDL_GetWindowPosition(window, &winx, &winy); 
        SDL_GetWindowSize(window, &winw, &winh);

        switch (st) {
            case HOLD_WIN:
                // SDL_SetWindowPosition(window, winx + deltax, winy + deltay);          
                SetWindowPos(getHWND(window), 0, winx+deltax,winy+deltay,0,0, SWP_NOSIZE | SWP_NOZORDER);
                break;
            case HOLD_SE:
                SDL_SetWindowSize(window, winw + deltax, winh + deltay);
                break;
            case HOLD_SW:
                SDL_SetWindowPosition(window, winx + deltax, winy);
                SDL_SetWindowSize(window, winw -deltax, winh + deltay);
                break;
            case HOLD_NE:
                SDL_SetWindowPosition(window, winx, winy + deltay);
                SDL_SetWindowSize(window, winw + deltax, winh - deltay);
                break;
            case HOLD_NW:
                SDL_SetWindowPosition(window, winx+deltax, winy + deltay);
                SDL_SetWindowSize(window, winw - deltax, winh - deltay);
                break;
            case HOLD_ALT:
                int newheight = deltax + rect.h;
                int newwidth = floor((double)newheight * imgRatio);
                rect.w = newwidth;
                rect.h = newheight;
                break;

            default:
                break;
            }
        
        UpdateImage();
        SDL_Delay(10);
    }
    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
}
