#ifndef LABEL_H
#define LABEL_H

#include "widget.h"
#include <stdint.h>

typedef struct FONT_DATA FONT_DATA;

typedef struct {
    WIDGET widget;
    uint32_t background_color;
    uint32_t tab_color;
    uint32_t title_color;
    uint32_t color;
    char* title;
    char* text;
    uint16_t font_size;
    FONT_DATA* font;
    size_t scroll_y;
} LABEL;

void DESTROY_LABEL(WIDGET* w);

LABEL* CREATE_LABEL(
    int x,int y,
    size_t w,size_t h
);

#endif