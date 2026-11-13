#ifndef LABEL_H
#define LABEL_H

#include "text.h"
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
    TEXT text;
} LABEL;

void DESTROY_LABEL(WIDGET* w);

LABEL* CREATE_LABEL(
    int x,int y,
    size_t w,size_t h
);

#endif