#include "label.h"
#include "graphics.h"
#include <stdlib.h>

void DESTROY_LABEL(WIDGET* w){
    if(!w) return;
    LABEL* label = (LABEL*)w;
    free(label);
};

void DRAW_LABEL(WIDGET* w,SDL_Surface* surface){
    if(!w || !surface) return;
    POINT abs = ABSOLUTE_WIDGET_POS(w);

    LABEL* label = (LABEL*)w;

    int offset_y = label->title? LABEL_TAB_HEIGHT : 0;

    if(label->title){
        fill_rect(
            surface,
            abs.x,abs.y,
            w->w,LABEL_TAB_HEIGHT,
            label->tab_color
        );

        TEXT text = (TEXT){
            .buffer = label->title,
            .color = label->title_color,
            .FLAGS = 0,
            .font = DEFAULT_FONT,
            .font_size = 8,
            .rect = (SDL_Rect){abs.x+4,abs.y+4,w->w-8,8},
            .scroll_y = 0
        };

        label->text.rect = (SDL_Rect){abs.x+5,abs.y+offset_y+5,w->w-10,w->h-10};
        DRAW_TEXT(
            &text,
            surface
        );
    }

    fill_rect(
        surface,
        abs.x,abs.y+offset_y,
        w->w,w->h,
        label->background_color
    );

    outline_rect(
        surface,
        abs.x,abs.y,
        w->w,w->h+offset_y,
        0xFF333333,2
    );

    DRAW_TEXT(&label->text,surface);
}

LABEL* CREATE_LABEL(
    int x,int y,
    size_t w,size_t h
){
    if(!TARGET_ROOT && !TARGET_WIDGET) return NULL;;

    LABEL* new_label = (LABEL*)calloc(1,sizeof(LABEL));
    if(!new_label) return NULL;

    INIT_WIDGET(
        &new_label->widget,LABEL_TYPE,
        x,y,w,h,
        DESTROY_LABEL,
        DRAW_LABEL,
        NULL
    );

    new_label->background_color = 0xFF1C140B;
    new_label->title_color = 0xFFFFB52E;
    new_label->tab_color = 0xFFA85D00;
    new_label->text.color = 0xFFFFB000;
    new_label->text.font_size = 8;

    APPEND_WIDGET((WIDGET*)new_label);

    return new_label;
};