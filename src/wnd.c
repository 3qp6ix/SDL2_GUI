#include "SDL2/SDL.h"
#include "graphics.h"
#include "wnd.h"
#include <stdlib.h>

void DESTROY_WND(WIDGET* w){
    if(!w) return;

    WND* window = (WND*)w;
    free(window);
};

void DRAW_WND(WIDGET* w,SDL_Surface* surface){
    if(!w || !surface) return;

    WND* wnd = (WND*)w;

    fill_rect(
        surface,
        w->x,w->y,
        w->w,WND_TAB_HEIGHT,
        wnd->tab_color
    );

    fill_rect(
        surface,
        w->x,w->y+WND_TAB_HEIGHT,
        w->w,w->h,
        wnd->background_color
    );

    outline_rect(
        surface,
        w->x,w->y,
        w->w,w->h+WND_TAB_HEIGHT,
        0xFF000000,2
    );
}

WND* CREATE_WND(
    int x,int y,
    size_t w,size_t h
){
    if(!TARGET_ROOT) return NULL;

    WND* new_wnd = (WND*)calloc(1,sizeof(WND));
    if(!new_wnd) return NULL;

    INIT_WIDGET(
        &new_wnd->widget,
        WND_TYPE,
        x,y,w,h,
        DESTROY_WND,
        DRAW_WND
    );

    new_wnd->background_color = 0xff23293b;
    new_wnd->tab_color = 0xff3f4e6e;

    APPEND_WIDGET((WIDGET*)new_wnd);

    return new_wnd;
};