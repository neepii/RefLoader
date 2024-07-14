#ifndef INIT_H_
#define INIT_H_
#include <windows.h>
#include <SDL2/sdl.h>
#define EXIT_FAIL -1
#define EXIT_QUIT 0
#define EXIT_IMAGEPATH_SYSTEM 1
#define EXIT_IMAGEPATH_URL 2
#define MAX_TEXT_LEN 2048

bool CH_InitSDL(void);
int CH_CreateMenu(char* );
void CH_Quit(void);
HWND getHWND(SDL_Window* );


#endif