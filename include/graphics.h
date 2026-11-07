#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "SDL2/SDL.h"

#include <stdint.h>

void clear_screen(SDL_Surface* surface,uint32_t color);

void fill_rect(
    SDL_Surface* surface,
    int x0,int y0,
    size_t w,size_t h,
    uint32_t color
);

void outline_rect(
    SDL_Surface* surface,
    int x0,int y0,
    size_t w,size_t h,
    uint32_t color,
    size_t stroke_width
);

#endif