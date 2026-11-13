#ifndef TEXT_INPUT_H
#define TEXT_INPUT_H

#include "widget.h"
#include <stdint.h>

typedef struct {
    WIDGET widget;
    uint32_t background_color;
    uint32_t tab_color;
    uint32_t title_color;
    uint16_t font_size;
    char* buffer;
    size_t capacity;
    char* title;
} TEXT_INPUT;

void DESTROY_TEXT_INPUT(WIDGET* w);

TEXT_INPUT* CREATE_TEXT_INPUT(
    int x,int y,
    size_t w,size_t h,
    size_t capacity
);

#endif