#include "graphics.h"

void clear_screen(SDL_Surface* surface,uint32_t color){
    if(!surface) return;

    uint32_t* pixels = (uint32_t*)surface->pixels;

    for(int i = 0; i < surface->w * surface->h; i++){
        pixels[i] = color;
    }
}

void fill_rect(
    SDL_Surface* surface,
    int x0,int y0,
    size_t w,size_t h,
    uint32_t color
){
    if(!surface || !surface->pixels) return;

    int s_w = surface->w;
    int s_h = surface->h;

    if(!s_w || !s_h || !w || !h) return;

    int64_t x1 = (int64_t)x0 + w;
    int64_t y1 = (int64_t)y0 + h;

    // Completely outside the surface
    if(x1 <= 0 || y1 <= 0 || x0 >= s_w || y0 >= s_h) return;

    // Clip rectangle to surface boundaries
    int sx = x0 < 0 ? 0 : x0;
    int sy = y0 < 0 ? 0 : y0;

    int ex = x1 > s_w ? s_w : (int)x1;
    int ey = y1 > s_h ? s_h : (int)y1;

    uint8_t* pixels = (uint8_t*)surface->pixels;

    for(int y = sy; y < ey; y++){
        uint32_t* row = (uint32_t*)(pixels + y * surface->pitch);

        for(int x = sx; x < ex; x++){
            row[x] = color;
        }
    }
}

void outline_rect(
    SDL_Surface* surface,
    int x0,int y0,
    size_t w,size_t h,
    uint32_t color,
    size_t stroke_width
){
    if(!surface || !w || !h || !stroke_width) return;

    fill_rect(
        surface,
        x0-(int)stroke_width,
        y0-(int)stroke_width,
        w+stroke_width*2,
        stroke_width,
        color
    );

    fill_rect(
        surface,
        x0+(int)w,
        y0,
        stroke_width,
        h,
        color
    );

    fill_rect(
        surface,
        x0-(int)stroke_width,
        y0+(int)h,
        w+stroke_width*2,
        stroke_width,
        color
    );

    fill_rect(
        surface,
        x0-(int)stroke_width,
        y0,
        stroke_width,
        h,
        color
    );
}