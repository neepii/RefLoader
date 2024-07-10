#include <SDL2/SDL.h>
#include <SDL2/sdl_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_syswm.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static SDL_Window * mwindow;
static SDL_Renderer * mrender;
static SDL_Texture * mtexture;
static SDL_Surface * msurface;

static const int mwW = 640;
static const int mwH = 480;

char * inputtext;
SDL_Rect inpRect;


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

static void update(void) {
    SDL_SetRenderDrawColor(mrender, 235, 235, 235, 235);
    SDL_RenderClear(mrender);
    SDL_RenderPresent(mrender);
}

void CH_Quit() {
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

bool CH_CreateMenu(char* inpDest) {
    int inpLen = 0;
    mwindow = SDL_CreateWindow("Crosshairy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mwW, mwH, SDL_WINDOW_OPENGL);
    mrender = SDL_CreateRenderer(mwindow, -1,SDL_RENDERER_SOFTWARE);
    mtexture = SDL_CreateTextureFromSurface(mrender, msurface);
    if (mwindow == NULL) {
        fprintf(stderr, "ERROR: Window is NULL. %s\n", SDL_GetError());
        return EXIT_FAILURE;
    };
    msurface = SDL_GetWindowSurface(mwindow);

    inputtext = (char*) malloc(sizeof(char) * 100);
    memset(inputtext, 0, strlen(inputtext));

    SDL_Color black = {0,0,0};
    SDL_Texture * inpTexture= SDL_CreateTextureFromSurface(mrender, msurface);
    inpRect.x = 25;
    inpRect.y = 25;
    inpRect.w = 430;
    inpRect.h = 30;
    TTF_Font * font = TTF_OpenFont("C:\\Windows\\Fonts\\Arial.ttf", 20);
    if (font == NULL) {
        fprintf(stderr, "ERROR: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    };
    if (!SDL_RenderCopy(mrender, mtexture, NULL, NULL)) {
        fprintf(stderr, "ERROR: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    };

    update();
    SDL_StartTextInput();
    bool done = false;
    bool loop = true;
    while(loop) {
        SDL_Event eve;
        bool inpFlag = false;
        while(SDL_PollEvent(&eve)) {
            switch (eve.type)
            {
            case SDL_QUIT:
                loop = false;
                break;
            case SDL_TEXTINPUT:
                strcat(inputtext, eve.text.text);
                inpLen++;
                inpFlag = true;
                break;
            case SDL_KEYDOWN:
                if (eve.key.keysym.sym == SDLK_BACKSPACE && inpLen > 0) {
                    inputtext[inpLen-1] = '\0';
                    inpLen--;
                    inpFlag = true;
                }
                else if (eve.key.keysym.sym == SDLK_RETURN) {
                    done = true;
                    inpFlag =true;
                }
                else if (eve.key.keysym.sym == SDLK_c && KMOD_CTRL) {
                    SDL_SetClipboardText(inputtext);
                    inpFlag = true;
                }
                else if (eve.key.keysym.sym == SDLK_v && KMOD_CTRL) {
                    strcpy(inputtext, SDL_GetClipboardText());
                    inpFlag = true;
                }

            default:
                break;
            }
        }

        if (inputtext && inpFlag) {
            if (done) {
                strcpy(inpDest, inputtext);
                loop = false;
            } else {
            update();
            SDL_Surface * inpSurf = TTF_RenderText_Blended(font, inputtext, black);
            inpTexture = SDL_CreateTextureFromSurface(mrender,inpSurf);
            inpRect.w = inpSurf->w;
            inpRect.h = inpSurf->h;

            SDL_RenderCopy(mrender, inpTexture, NULL, &inpRect);

            
            SDL_DestroyTexture(inpTexture);
            SDL_FreeSurface(inpSurf);
            }
            
        }

        SDL_RenderPresent(mrender);
        SDL_Delay(10);

    } // loop

    SDL_StopTextInput();
    free(inputtext);
    TTF_CloseFont(font);
    SDL_DestroyTexture(mtexture);
    SDL_DestroyWindowSurface(mwindow);
    SDL_DestroyWindow(mwindow);


    return EXIT_SUCCESS;
}