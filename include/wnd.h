#ifndef WND_H
#define WND_H

#include "widget.h"
#include <stdint.h>

typedef struct {
    WIDGET widget;
    uint32_t background_color;
    uint32_t tab_color;
} WND;

WND* CREATE_WND(
    int x,int y,
    size_t w,size_t h
);

void DESTROY_WND(WIDGET* w);

#endif