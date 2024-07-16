#include <SDL2/SDL.h>
#include <SDL2/sdl_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_mouse.h>
#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "curl.h"
#include "init.h"



static SDL_Window * mwindow;
static SDL_Renderer * mrender;
static SDL_Texture * mtexture;
static SDL_Texture * foldertex;
static SDL_Surface * msurface;
static SDL_Surface * foldersrf;
static TTF_Font * font;


static SDL_Rect textbox = {30,420,580,25};
static SDL_Rect buttondialog = {133,30,374,374};
static SDL_Rect bar = {0,0,640,30};
static const int mwW = 640;
static const int mwH = 480;

int inpPrefixLen[MAX_TEXT_LEN+1];
bool inpFlag;
int inpLen;
static const int cursorpadding = 3;
bool dialogpressed = false;

int exit_code = 0;

char * inputtext;
SDL_Rect inpRect;
char ** dests;
int destLen;

void freeDests() {
    for (int i = 0; i < destLen; i++) {
        free(dests[i]);
    }
    free(dests);
}

void allocDests(int count) {
    destLen = count;
    dests = (char**) malloc(sizeof(char*) * destLen);
    for (int i = 0; i < destLen; i++)
    {
        dests[i] = (char*) malloc(sizeof(char)*MAX_TEXT_LEN);
        memset(dests[i], 0, MAX_TEXT_LEN);
    }
}

bool EnteredRect(int * x, int * y, SDL_Rect * rect) {
    return (*x >= rect->x &&
            *y >= rect->y &&
            *x <= (rect->w +rect->x) &&
            *y <= (rect->h + rect->y)) ? true : false;
}

HWND getHWND(SDL_Window * window) {
    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version);
    SDL_GetWindowWMInfo(window, &wminfo);

    return wminfo.info.win.window;
};

static void addChar(char * dest, int pos, char* seed) {
    char temp[512];
    memset(temp,0,512);

    strncpy(temp, dest, pos);
    int len = strlen(temp);
    strcpy(temp+len, seed);
    len += strlen(seed);
    strcpy(temp+len, dest+pos);

    strcpy(dest, temp);

}

static int getTextLen(char * text) {
    int w;
    TTF_SizeText(font, text, &w, NULL);
    return w;
}

static void UpdatePrefix(int start, int end, char * inputtext) {
    char temp[MAX_TEXT_LEN];
    for (int i = start; i<=end; i++) {
        strncpy(temp, inputtext, inpLen);
        temp[i] = '\0';
        inpPrefixLen[i] = getTextLen(temp);
    }
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

};

static void update(void) {
    SDL_Rect Rects[3] = {textbox, bar};

    SDL_SetRenderDrawColor(mrender, 255, 255, 255, 255);
    SDL_RenderClear(mrender);

    SDL_SetRenderDrawColor(mrender, 205,205,205,255);
    SDL_RenderDrawRects(mrender,Rects, 2);
    (dialogpressed) ? SDL_SetRenderDrawColor(mrender, 205,205,205,255) : SDL_SetRenderDrawColor(mrender, 255, 255, 255, 255);
    SDL_RenderDrawRect(mrender, &buttondialog);


    foldertex = SDL_CreateTextureFromSurface(mrender, foldersrf);
    SDL_RenderCopy(mrender, foldertex, NULL, &buttondialog);

    SDL_DestroyTexture(foldertex);
    SDL_RenderPresent(mrender);
};

void CH_Quit() {
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
};





int CH_CreateMenu(void) {

    // HWND hwnd = getHWND(mwindow);

    inpLen = 0;
    int inpShift = 0;
    int cursorX = inpLen;
    mwindow = SDL_CreateWindow("Ref", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mwW, mwH, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
    mrender = SDL_CreateRenderer(mwindow, -1,SDL_RENDERER_SOFTWARE);
    mtexture = SDL_CreateTextureFromSurface(mrender, msurface);
    if (mwindow == NULL) {
        fprintf(stderr, "ERROR: Window is NULL. %s\n", SDL_GetError());
        return EXIT_FAILURE;
    };
    msurface = SDL_GetWindowSurface(mwindow);

    SDL_Surface * tempsrf = IMG_Load("res/folder.png");
    foldersrf = SDL_ConvertSurface(tempsrf, msurface->format, 0);
    SDL_FreeSurface(tempsrf);

    char inputtext[MAX_TEXT_LEN];
    memset(inputtext, 0, strlen(inputtext));
    inpPrefixLen[0] = 0;
    SDL_Color black = {0,0,0};
    SDL_Texture * inpTexture= SDL_CreateTextureFromSurface(mrender, msurface);
    inpRect.x = textbox.x;         
    inpRect.y = textbox.y;
    inpRect.w = 0;
    inpRect.h = 0;
    SDL_Rect cursorrect;
    font = TTF_OpenFont("C:\\Windows\\Fonts\\Arial.ttf", 20);
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
    bool holding = false;
    bool frombackspace = false;
    int x,y;
    POINT cursorpos;


    while(loop) {
        SDL_Event eve;
        inpFlag = false;

        while(SDL_PollEvent(&eve)) {

            switch (eve.type)
            {
            case SDL_QUIT:
                loop = false;
                exit_code = EXIT_QUIT;
                break;
            case SDL_TEXTINPUT:
                if (inpLen + 1 <= MAX_TEXT_LEN) {
                    char ch[2] = {eve.text.text[0], '\0'};
                    addChar(inputtext, cursorX, ch);
                    inpLen++;
                    cursorX++;
                    UpdatePrefix(cursorX, inpLen, inputtext);
                    inpFlag = true;
                }
                break;
            case SDL_KEYDOWN:
                switch (eve.key.keysym.sym) {
                    case SDLK_BACKSPACE:
                        if (inpLen >= 0 && cursorX != 0) {
                            for (int i = cursorX; i<=inpLen; i++) {
                                inputtext[i-1] = inputtext[i];
                            }
                            inputtext[inpLen] = '\0';

                            UpdatePrefix(cursorX, inpLen,inputtext);

                            inpLen--;
                            cursorX--;
                            inpFlag = true;
                            frombackspace =true;
                        }
                        break;
                    case SDLK_DELETE:
                        if (inpLen >= 0 && cursorX != inpLen && inpLen != 0) {
                            inputtext[inpLen] = '\0';
                            for (int i = cursorX; i<=inpLen; i++) {
                                inputtext[i] = inputtext[i+1];
                            }
                            UpdatePrefix(cursorX, inpLen, inputtext);
                            inpFlag = true;
                            inpLen--;
                        }
                        break;

                    case SDLK_RETURN:
                        if (*inputtext == '"' && inputtext[inpLen-1] == '"') {
                            memmove(inputtext, inputtext+1, strlen(inputtext)-2);
                            inputtext[strlen(inputtext)-2 ]= '\0';
                        }
                        if (isHTTPS(inputtext)) {
                            exit_code = EXIT_IMAGEPATH_URL;
                        } else {
                            exit_code = EXIT_IMAGEPATH_SYSTEM;
                        }
                        allocDests(1);
                        strncpy(dests[0], inputtext, inpLen);
                        done = true;
                        inpFlag =true;
                        break;
                        
                    case SDLK_c:
                        if (KMOD_CTRL) {
                            SDL_SetClipboardText(inputtext);
                        }
                        break;
                    case SDLK_v:
                        if(eve.key.keysym.mod & KMOD_CTRL) {
                            char cliptext[MAX_TEXT_LEN];
                            strcpy(cliptext, SDL_GetClipboardText());
                            int clipLen = strlen(cliptext);
                            if ((inpLen+clipLen) < MAX_TEXT_LEN) { 
                                addChar(inputtext, cursorX, cliptext);
                                inpLen += clipLen;
                                cursorX += clipLen;
                                UpdatePrefix(0, inpLen, inputtext);
                                inpFlag = true;
                            }
                        }
                        break; 
                    case SDLK_RIGHT:
                        if (cursorX < inpLen) {
                            cursorX++;
                            inpFlag = true;
                        }
                        break;
                    case SDLK_LEFT:
                        if (cursorX > 0) {
                            cursorX--;
                            inpFlag = true;
                        }
                        break;
                    case SDLK_END:
                        cursorX = inpLen;
                        inpFlag = true;
                        break;
                    case SDLK_HOME:
                        cursorX = 0;
                        inpFlag = true;
                        break;
                    }
                break;
            case SDL_MOUSEBUTTONDOWN:
                dialogpressed = true;
                GetCursorPos(&cursorpos);
                if (EnteredRect(&x,&y,&bar)) {
                    holding = true;
                }          
                

                break;
            case SDL_MOUSEBUTTONUP:
                holding = false;
                dialogpressed = false;
                if (EnteredRect(&x,&y,&buttondialog)) {
                        OPENFILENAME dialog; 
                        CHAR szfile[100];
                        ZeroMemory(&dialog, sizeof(dialog));
                        dialog.lStructSize = sizeof(dialog);
                        dialog.hwndOwner = NULL;
                        dialog.lpstrFile = szfile;
                        dialog.lpstrFile[0] = '\0';
                        dialog.nMaxFile = sizeof(szfile);
                        dialog.lpstrFilter = "All Files\0*.*\0\0";
                        dialog.nFilterIndex = 1;
                        dialog.lpstrFileTitle = NULL;
                        dialog.nMaxFileTitle = 0;
                        dialog.lpstrInitialDir = NULL;
                        dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_ALLOWMULTISELECT;
                        if (GetOpenFileName(&dialog)) {
                            int count = 0;
                            char * catalog = dialog.lpstrFile;
                            char * ptr = catalog + dialog.nFileOffset;
                            while (*ptr) {
                                count++;
                                ptr += strlen(ptr) + 1;
                            }
                            allocDests(count);
                            if (count == 1) {
                                strncpy(dests[0], catalog, strlen(catalog));
                            }
                            else {
                                ptr = catalog + dialog.nFileOffset;
                                allocDests(count);
                                for (int i = 0; i < count; i++)
                                {
                                    char temp[256];
                                    int len = 0;
                                    strncpy(temp, catalog, dialog.nFileOffset);
                                    len = dialog.nFileOffset;
                                    temp[len-1] = '\\';
                                    strncpy(temp+len, ptr, strlen(ptr));
                                    len += strlen(ptr);
                                    strncpy(temp+len, "\0", 1);
                                    ptr += strlen(ptr) + 1;
                                    strncpy(dests[i], temp, strlen(temp));
                                }
                            }
                            done = true;
                            inpFlag = true;
                            exit_code = EXIT_IMAGEPATH_SYSTEM;
                        } else {
                            printf("PATHSIZE bypassed or just canceled\n");
                        }
                        
                    }
                break;
            default:
                break;
            }
        }
        POINT temp = cursorpos;
        GetCursorPos(&cursorpos);

        int deltax = (int)(cursorpos.x - temp.x);
        int deltay = (int)(cursorpos.y - temp.y);

        int winx, winy;
        SDL_GetWindowPosition(mwindow, &winx, &winy); // make it a function


        if (inpFlag && done) {
            loop = false;
        }
        else if (holding) {
            printf("%d %d\n", deltax,deltay);
            SetWindowPos(getHWND(mwindow), 0, deltax + winx, deltay + winy, 0,0,SWP_NOSIZE | SWP_NOZORDER);
        }
        else if (inpFlag) {
            update();
            SDL_Surface * inpSurf = TTF_RenderText_Blended(font, inputtext, black);
            inpTexture = SDL_CreateTextureFromSurface(mrender,inpSurf);

            SDL_Rect hiderect = {0,0,30,480};
            SDL_SetRenderDrawColor(mrender, 255,255,255,255);

            int delta;
            if (frombackspace && inpSurf == 0x0) {
                inpRect.w = 0; inpRect.h = 0;
                delta = 0;
            } else {
                delta = inpSurf->w - inpRect.w;
                inpRect.w = inpSurf->w; inpRect.h = inpSurf->h;
            }

            bool isBig = (delta > textbox.w) ? true : false;

            if (inpRect.w > textbox.w && !isBig) {
                inpRect.x -= delta;
                inpShift += delta;
            }
            else if (inpRect.w > textbox.w && isBig) {
                inpRect.x -= delta - textbox.w;
                inpShift += delta - textbox.w;
            } 
            else {
                inpRect.x = textbox.x;
                inpShift = 0;
            }

            cursorrect.x = inpPrefixLen[cursorX]+30-inpShift; cursorrect.y = textbox.y + cursorpadding;
            cursorrect.w = 1; cursorrect.h = textbox.h - (2 * cursorpadding);
            SDL_FillRect(msurface, &cursorrect, SDL_MapRGB(msurface->format, 150, 150,150));
            SDL_RenderCopy(mrender, inpTexture, NULL, &inpRect);

            SDL_RenderFillRect(mrender, &hiderect);

            SDL_DestroyTexture(inpTexture);
            SDL_FreeSurface(inpSurf);
            }
        SDL_RenderPresent(mrender);
        SDL_Delay(10);

    } // loop

    SDL_StopTextInput();
    TTF_CloseFont(font);
    SDL_DestroyTexture(mtexture);
    SDL_DestroyRenderer(mrender);
    SDL_DestroyWindowSurface(mwindow);
    SDL_DestroyWindow(mwindow);


    return exit_code;
}