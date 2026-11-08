#include "widget.h"

#include <stdlib.h>

POINT ABSOLUTE_WIDGET_POS(WIDGET* w){
    if(!w) return (POINT){0,0};
    int x = w->x;
    int y = w->y;
    WIDGET* parent = w->parent;
    while(parent){
        x += parent->x;
        y += parent->y;
        parent = parent->parent;
    }

    return (POINT){x,y};
}

void INIT_WIDGET(
    WIDGET* widget,
    WIDGET_TYPE type,
    int x,int y,
    size_t w,size_t h,
    void (*DESTROY)(WIDGET*),
    void (*DRAW)(WIDGET*,SDL_Surface*)
){
    if(!widget) return;
    *widget = (WIDGET){
        .type = type,
        .x = x,.y = y,
        .w = w,.h = h,
        .children.head = NULL,
        .children.tail = NULL,
        .parent = NULL,
        .DESTROY = DESTROY,
        .DRAW = DRAW
    };
}

void APPEND_WIDGET(WIDGET* w){
    if(!w) return;

    if(TARGET_WIDGET){
        WIDGET_LIST_APPEND(&TARGET_WIDGET->children,w);
        w->parent = TARGET_WIDGET;
    }else{
        WIDGET_LIST_APPEND(TARGET_ROOT,w);
    }
}

bool WIDGET_LIST_APPEND(WIDGET_LIST* list,WIDGET* widget){
    if(!list || !widget) return false;

    if(!list->head){
        widget->prev = NULL;
        list->head = widget;
    }else{
        list->tail->next = widget;
        widget->prev = list->tail;
    }

    list->tail = widget;
    widget->next = NULL;
    
    return true;
}

void WIDGET_LIST_FREE_WIDGETS(WIDGET_LIST* list){
    if(!list) return;
    WIDGET* current = list->head;
    while(current){
        WIDGET* next = current->next;

        WIDGET_LIST_FREE_WIDGETS(&current->children);
        if(current->DESTROY){
            current->DESTROY(current);
        }

        current = next;
    }
}

void WIDGET_LIST_DRAW_WIDGETS(WIDGET_LIST* list,SDL_Surface* surface){
     if(!list) return;
    WIDGET* current = list->head;
    while(current){
        if(current->DRAW){
            current->DRAW(current,surface);
        }

        WIDGET_LIST_DRAW_WIDGETS(&current->children,surface);

        current = current->next;
    }
}

WIDGET_MANAGER* CREATE_WIDGET_MANAGER(){
    return calloc(1,sizeof(WIDGET_MANAGER));
}

void DESTROY_WIDGET_MANAGER(WIDGET_MANAGER* manager){
    if(!manager) return;
    WIDGET_LIST_FREE_WIDGETS(&manager->root);
    free(manager);
}

WIDGET_LIST* TARGET_ROOT = NULL;
WIDGET* TARGET_WIDGET = NULL;