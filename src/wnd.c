#include "graphics.h"
#include "wnd.h"
#include "text.h"
#include <stdlib.h>

#include <stdio.h>

uint8_t WND_HANDLE_EVENT(WIDGET* w,SDL_Event* e){
    if(!e) return false;
    uint8_t FLAGS = 0;

    POINT abs = ABSOLUTE_WIDGET_POS(w);

    int mx = e->motion.x;
    int my = e->motion.y;
    bool IN_RECT = POINT_IN_RECT(mx,my,(SDL_Rect){abs.x,abs.y,w->w,w->h+WND_TAB_HEIGHT});
    bool IN_TAB = POINT_IN_RECT(mx,my,(SDL_Rect){abs.x,abs.y,w->w,WND_TAB_HEIGHT});

    switch(e->type){
        case SDL_MOUSEMOTION:
            if(w->dragging){
                w->x = mx;
                w->y = my;
                FLAGS |= EVENT_HANDLED | REDRAW_EVERYTHING;
            }

            if(IN_RECT){
                FLAGS |= EVENT_HANDLED;
                if(!w->hovered){
                    w->hovered = true;
                    FLAGS |= REDRAW;
                } 
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(e->button.button==SDL_BUTTON_LEFT){
                if(!w->parent && IN_TAB){
                    FLAGS |= EVENT_HANDLED;
                    if(!w->dragging){
                        w->dragging = true;
                    }
                }
            }
            break;
    }

    return FLAGS;
}

void DESTROY_WND(WIDGET* w){
    if(!w) return;

    WND* window = (WND*)w;
    free(window);
};

void DRAW_WND(WIDGET* w,SDL_Surface* surface){
    if(!w || !surface) return;

    WND* wnd = (WND*)w;

    POINT abs = ABSOLUTE_WIDGET_POS(w);

    fill_rect(
        surface,
        abs.x,abs.y,
        w->w,WND_TAB_HEIGHT,
        w->hovered? 0xFF69765A : wnd->tab_color
    );

    if(wnd->title){
        DRAW_TEXT(
            &(TEXT){
                .buffer = wnd->title,
                .color = w->hovered? 0xFFE8DFC2 : wnd->title_color,
                .FLAGS = 0,
                .font = DEFAULT_FONT,
                .font_size = 8,
                .rect = (SDL_Rect){abs.x+8,abs.y+8,8,w->w}
            },
            surface
        );
    }

    fill_rect(
        surface,
        abs.x,abs.y+WND_TAB_HEIGHT,
        w->w,w->h,
        wnd->background_color
    );

    outline_rect(
        surface,
        abs.x,abs.y,
        w->w,w->h+WND_TAB_HEIGHT,
        0xFF000000,2
    );
}

WND* CREATE_WND(
    int x,int y,
    size_t w,size_t h
){
    if(!TARGET_ROOT && !TARGET_WIDGET) return NULL;

    WND* new_wnd = (WND*)calloc(1,sizeof(WND));
    if(!new_wnd) return NULL;

    INIT_WIDGET(
        &new_wnd->widget,
        WND_TYPE,
        x,y,w,h,
        DESTROY_WND,
        DRAW_WND,
        WND_HANDLE_EVENT
    );

    new_wnd->title = "Window";
    new_wnd->background_color = 0xff23293b;
    new_wnd->tab_color = 0xff3f4e6e;
    new_wnd->title_color = 0xFFFFFFFF;

    APPEND_WIDGET((WIDGET*)new_wnd);

    return new_wnd;
};