#ifndef INIT_H_
#define INIT_H_
#include <windows.h>
#include <SDL2/sdl.h>

bool CH_InitSDL(void);
bool CH_CreateMenu(void);
void CH_Quit(void);
HWND getHWND(SDL_Window* );


#endif