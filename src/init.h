#ifndef INIT_H_
#define INIT_H_
#include <windows.h>
#include <SDL2/sdl.h>
#define EXIT_FAIL -1
#define EXIT_QUIT 0
#define EXIT_IMAGEPATH_SYSTEM 1
#define EXIT_IMAGEPATH_URL 2
#define MAX_TEXT_LEN 2048
#define MAX(a,b) (a > b) ? a : b
#define LEN(arr) (sizeof(arr) / sizeof(*arr))
#define SLEN(str) !

extern char** dests;
extern int destLen;

bool CH_InitSDL(void);
int CH_CreateMenu(void);
void CH_Quit(void);
void freeDests(void);
void allocDests(int count);
HWND getHWND(SDL_Window* );
size_t getPath(char*, size_t);
SDL_Surface * LoadIMGFromRC(int);


#endif