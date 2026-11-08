#ifndef WIDGET_H
#define WIDGET_H

#define WND_TAB_HEIGHT 24

#include <stddef.h>
#include <stdbool.h>

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
    WND_TYPE
} WIDGET_TYPE;

struct WIDGET{
    WIDGET_TYPE type;
    int x,y;
    size_t w,h;

    WIDGET_LIST children;

    void (*DESTROY)(WIDGET*);
    void (*DRAW)(WIDGET*,SDL_Surface*);

    WIDGET* parent;
    WIDGET* prev;
    WIDGET* next;
};

POINT ABSOLUTE_WIDGET_POS(WIDGET* w);

void INIT_WIDGET(
    WIDGET* widget,
    WIDGET_TYPE type,
    int x,int y,
    size_t w,size_t h,
    void (*DESTROY)(WIDGET*),
    void (*DRAW)(WIDGET*,SDL_Surface*)
);

void APPEND_WIDGET(WIDGET* w);

bool WIDGET_LIST_APPEND(WIDGET_LIST* list,WIDGET* widget);
void WIDGET_LIST_FREE_WIDGETS(WIDGET_LIST* list);
void WIDGET_LIST_DRAW_WIDGETS(WIDGET_LIST* list,SDL_Surface* surface);

typedef struct {
    WIDGET_LIST root;
    SDL_Surface* surface;
} WIDGET_MANAGER;

WIDGET_MANAGER* CREATE_WIDGET_MANAGER(void);
void DESTROY_WIDGET_MANAGER(WIDGET_MANAGER* manager);

extern WIDGET_LIST* TARGET_ROOT;
extern WIDGET* TARGET_WIDGET;

#endif
