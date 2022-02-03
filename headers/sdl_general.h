#ifndef SDL_GENERAL_PC
#define SDL_GENERAL_PC

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

void putText(SDL_Renderer* renderer, std::string text, int x, int y, int size);

#endif