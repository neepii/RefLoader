#include <stdbool.h>
#include <windows.h>
#include <SDL2/SDL.h>

#ifndef FUNCS_H_
#define FUNCS_H_


bool init();
bool load();
void start();
void quit();
bool MakeWindowTransparent(SDL_Window * window, COLORREF color);

#endif