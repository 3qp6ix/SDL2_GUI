#include "graphics.h"

void clear_screen(SDL_Surface* surface,uint32_t color){
    if(!surface) return;
    uint32_t* pixels = (uint32_t*)surface->pixels;
    for(int i=0;i<surface->w*surface->h;i++){
        pixels[i] = color;
    } 
}

void fill_rect(
    SDL_Surface* surface,
    int x0,int y0,
    size_t w,size_t h,
    uint32_t color
){
    if(!surface) return;

    int s_w = surface->w;
    int s_h = surface->h;

    if(!s_w || !s_h) return;
    if(!surface->pixels || w==0 || h==0) return;

    int x1 = x0 + (int)w;
    int y1 = y0 + (int)h;

    if(x0>=s_w || y0>=s_h) return;
    if(x1<=0 || y1<=0) return;

    if(x0<0) x0=0;
    if(y0<0) y0=0;
    if(x1>s_w) x1=s_w;
    if(y1>s_h) y1=s_h;

    uint32_t* pixels = (uint32_t*)surface->pixels;

    for(int y=y0;y<y1;y++){
        for(int x=x0;x<x1;x++){
            pixels[y*s_w+x] = color;
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
    if(!surface || (w==0 || h==0)) return;
    if(x0>=surface->w || y0>=surface->h) return;

    int x1 = x0 + w;
    int y1 = y0 + h;
    
    if(x1 < 0 || y1 < 0) return;

    fill_rect(
        surface,
        x0-stroke_width,y0-stroke_width,
        w+stroke_width*2,
        stroke_width,
        color
    );

    fill_rect(
        surface,
        x0+w,y0,
        stroke_width,
        h+stroke_width,
        color
    );

    fill_rect(
        surface,
        x0-stroke_width,
        y0+h,
        w+stroke_width,
        stroke_width,
        color
    );

    fill_rect(
        surface,
        x0-stroke_width,
        y0,
        stroke_width,
        h,
        color
    );
}