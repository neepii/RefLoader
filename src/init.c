#include <SDL2/SDL.h>
#include <SDL2/sdl_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_syswm.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define TEXT_MAX_SIZE 100

static SDL_Window * mwindow;
static SDL_Renderer * mrender;
static SDL_Texture * mtexture;
static SDL_Surface * msurface;

static const int mwW = 640;
static const int mwH = 480;

char * inputtext;


HWND getHWND(SDL_Window * window) {
    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version);
    SDL_GetWindowWMInfo(window, &wminfo);

    return wminfo.info.win.window;
}

bool CH_InitSDL() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "ERROR: Init Failed: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    int flags = IMG_INIT_PNG;
    if ( !( IMG_Init( flags ) & flags ) ) {
        fprintf(stderr, "ERROR: Cant Init an IMG. %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    if (TTF_Init() != 0 ) {
        fprintf(stderr, "ERROR: Init Failed: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}

void update(void) {
    SDL_SetRenderDrawColor(mrender, 235, 235, 235, 235);
    SDL_RenderClear(mrender);
    SDL_RenderPresent(mrender);
}

void CH_Quit() {
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

bool CH_CreateMenu() {
    mwindow = SDL_CreateWindow("Crosshairy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mwW, mwH, SDL_WINDOW_OPENGL);
    mrender = SDL_CreateRenderer(mwindow, -1,SDL_RENDERER_SOFTWARE);
    mtexture = SDL_CreateTextureFromSurface(mrender, msurface);
    if (mwindow == NULL) {
        fprintf(stderr, "ERROR: Window is NULL. %s\n", SDL_GetError());
        return EXIT_FAILURE;
    };
    msurface = SDL_GetWindowSurface(mwindow);

    update();
    
    int padding = 25;
    SDL_Rect innerwhite = {padding, padding ,mwW-(2* padding), mwH - (2*padding)};
    // SDL_SetRenderDrawColor(mrender, 245,245,245,245);
    // SDL_FillRect(msurface, &innerwhite, SDL_MapRGBA(msurface->format,255,255,255,255));
    // SDL_RenderPresent(mrender);

    SDL_StartTextInput();

    inputtext = (char*) malloc(sizeof(char) * TEXT_MAX_SIZE);
    memset(inputtext, 0, strlen(inputtext));
    int inpLen = 0;
    int tempLen;
    SDL_Texture * inpTexture= SDL_CreateTextureFromSurface(mrender, msurface);
    SDL_Rect inpRect = {innerwhite.x,innerwhite.y,innerwhite.w,innerwhite.h-mwH/2};
    TTF_Font * font = TTF_OpenFont("C:\\Windows\\Fonts\\Arial.ttf", 255);
    if (font == NULL) {
        fprintf(stderr, "ERROR: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    };
    SDL_Color black = {0,0,0};

    if (!SDL_RenderCopy(mrender, mtexture, NULL, NULL)) {
        fprintf(stderr, "ERROR: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    };

    bool loop = true;
    while(loop) {
        SDL_Event eve;
        tempLen = inpLen;
        while(SDL_PollEvent(&eve)) {
            switch (eve.type)
            {
            case SDL_QUIT:
                loop = false;
                break;
            case SDL_TEXTINPUT:
                strcat(inputtext, eve.text.text);
                inpLen++;
                break;
            case SDL_KEYDOWN:
                if (eve.key.keysym.sym == SDLK_BACKSPACE && inputtext) {
                    inputtext[strlen(inputtext)-1] = '\0';
                    inpLen--;
                }
                if (eve.key.keysym.sym == SDLK_KP_ENTER && inputtext) {
                    
                }

            default:
                break;
            }
        }

        if (inputtext && tempLen != inpLen) {
            update();
            SDL_Surface * inpSurf = TTF_RenderText_Blended(font, inputtext, black);
            inpTexture = SDL_CreateTextureFromSurface(mrender,inpSurf);
            SDL_RenderCopy(mrender, inpTexture, NULL, &inpRect);
            
            SDL_DestroyTexture(inpTexture);
            SDL_FreeSurface(inpSurf);
        }

        SDL_RenderPresent(mrender);
        SDL_Delay(10);
    }

    SDL_StopTextInput();
    free(inputtext);
    TTF_CloseFont(font);
    SDL_DestroyTexture(mtexture);
    SDL_DestroyWindowSurface(mwindow);
    SDL_DestroyWindow(mwindow);

    return EXIT_SUCCESS;
}