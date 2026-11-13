#include "text_input.h"
#include "text.h"
#include "graphics.h"
#include <stdlib.h>

void DESTROY_TEXT_INPUT(WIDGET* w){
    if(!w) return;
    TEXT_INPUT* text_input = (TEXT_INPUT*)w;
    free(text_input->buffer);
    free(text_input);
};

void DRAW_TEXT_INPUT(WIDGET* w,SDL_Surface* surface){
    if(!w || !surface) return;

    POINT abs = ABSOLUTE_WIDGET_POS(w);
    TEXT_INPUT* text_input = (TEXT_INPUT*)w;

    fill_rect(
        surface,
        abs.x,abs.y,
        w->w,TEXT_INPUT_TAB_HEIGHT,
        text_input->tab_color
    );

    TEXT text = {
        .buffer = text_input->title,
        .color = text_input->title_color,
        .FLAGS = CLIP | WRAP,
        .font = DEFAULT_FONT,
        .font_size = 8,
        .rect = (SDL_Rect){abs.x+2,abs.y+2,w->w,TEXT_INPUT_TAB_HEIGHT-4},
        .scroll_y = 0
    };

    DRAW_TEXT(
        &text,
        surface
    );

    fill_rect(
        surface,
        abs.x,abs.y+TEXT_INPUT_TAB_HEIGHT,
        w->w,w->h,
        text_input->background_color
    );
}

TEXT_INPUT* CREATE_TEXT_INPUT(
    int x,int y,
    size_t w,size_t h,
    size_t capacity
){
    if(!TARGET_ROOT && !TARGET_WIDGET) return NULL;
    TEXT_INPUT* text_input = (TEXT_INPUT*)calloc(1,sizeof(TEXT_INPUT));
    if(!text_input) return NULL;

    char* buffer = calloc(capacity+1,sizeof(char));
    if(!buffer){
        free(text_input);
        return NULL;
    }

    INIT_WIDGET(
        &text_input->widget,
        TEXT_INPUT_TYPE,
        x,y,w,h,
        DESTROY_TEXT_INPUT,
        DRAW_TEXT_INPUT,
        NULL
    );

    text_input->capacity = capacity;
    text_input->buffer = buffer;
    text_input->background_color = 0xFFD6D6D0;
    text_input->title = "INPUT";
    text_input->tab_color = 0xFF668C8B;
    text_input->title_color = 0xFF343A3A;

    APPEND_WIDGET((WIDGET*)text_input);

    return text_input;
};