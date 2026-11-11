#ifndef TEXT_H
#define TEXT_H

#include "SDL2/SDL.h"
#include <stdint.h>
#include <stddef.h>

#define SIZE 128

#define WRAP 0b00000001
#define CLIP 0b00000010
#define CENTER_X 0b00000100
#define CENTER_Y 0b00001000

typedef struct {
    uint8_t w,h,i;
    uint8_t* bitmap;
} GLYPH;

typedef struct FONT_DATA{
    size_t HOW_MANY;
    uint16_t line_height;
    GLYPH glyphs[SIZE];
} FONT_DATA;

typedef struct {
    SDL_Rect rect;
    char* buffer;
    FONT_DATA* font;
    uint16_t font_size;
    uint32_t color;
    uint8_t FLAGS;
    size_t scroll_y;
    size_t w,h;
} TEXT;

void UPDATE_TEXT(TEXT* text);

FONT_DATA* GET_FONT_DATA(const char* filepath);
void DESTROY_FONT_DATA(FONT_DATA* f_data);

void DRAW_TEXT(TEXT* text,SDL_Surface* surface);

extern FONT_DATA* DEFAULT_FONT;

#endif