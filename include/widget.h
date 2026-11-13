#ifndef WIDGET_H
#define WIDGET_H

#define WND_TAB_HEIGHT 24
#define LABEL_TAB_HEIGHT 16
#define TEXT_INPUT_TAB_HEIGHT 12

#define EVENT_HANDLED 0b00000001
#define REDRAW 0b00000010
#define REDRAW_EVERYTHING 0b00000100

#include <stddef.h>
#include <stdbool.h>
#include "SDL2/SDL.h"

typedef struct SDL_Surface SDL_Surface;

typedef struct WIDGET WIDGET;
typedef struct WIDGET_LIST WIDGET_LIST;

typedef struct {
    int x,y;
} POINT;

struct WIDGET_LIST{
    WIDGET* head;
    WIDGET* tail;
};

typedef enum {
    WND_TYPE,
    LABEL_TYPE,
    TEXT_INPUT_TYPE
} WIDGET_TYPE;

struct WIDGET{
    WIDGET_TYPE type;
    int x,y;
    size_t w,h;

    WIDGET_LIST children;

    void (*DESTROY)(WIDGET*);
    void (*DRAW)(WIDGET*,SDL_Surface*);
    uint8_t (*HANDLE_EVENT)(WIDGET*,SDL_Event* e);

    bool hovered;
    bool dragging;

    WIDGET* parent;
    WIDGET* prev;
    WIDGET* next;
};

bool POINT_IN_RECT(int x,int y,SDL_Rect rect);
POINT ABSOLUTE_WIDGET_POS(WIDGET* w);

void INIT_WIDGET(
    WIDGET* widget,
    WIDGET_TYPE type,
    int x,int y,
    size_t w,size_t h,
    void (*DESTROY)(WIDGET*),
    void (*DRAW)(WIDGET*,SDL_Surface*),
    uint8_t (*HANDLE_EVENT)(WIDGET*,SDL_Event*)
);

void APPEND_WIDGET(WIDGET* w);

bool WIDGET_LIST_APPEND(WIDGET_LIST* list,WIDGET* widget);
void WIDGET_LIST_FREE_WIDGETS(WIDGET_LIST* list);
void WIDGET_LIST_DRAW_WIDGETS(WIDGET_LIST* list,SDL_Surface* surface);

typedef struct {
    WIDGET* widget;
    uint8_t flags;
} EVENT_DATA;

EVENT_DATA WIDGET_LIST_HANDLE_EVENT(WIDGET_LIST* list,SDL_Event* e);

typedef struct {
    WIDGET_LIST root;
    SDL_Surface* surface;
    WIDGET* last_hovered;
    WIDGET* dragging_widget;
} WIDGET_MANAGER;

WIDGET_MANAGER* CREATE_WIDGET_MANAGER(void);
void DESTROY_WIDGET_MANAGER(WIDGET_MANAGER* manager);
void WIDGET_MANAGER_DRAW_WIDGETS(WIDGET_MANAGER* manager);
void WIDGET_MANAGER_HANDLE_EVENT(WIDGET_MANAGER* manager,SDL_Event* e);

extern WIDGET_LIST* TARGET_ROOT;
extern WIDGET* TARGET_WIDGET;

#endif
