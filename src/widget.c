#include "widget.h"
#include <stdlib.h>
#include <stdbool.h>

bool POINT_IN_RECT(int x, int y, SDL_Rect rect) {
    return x >= rect.x &&
           y >= rect.y &&
           x < rect.x + rect.w &&
           y < rect.y + rect.h;
}

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
    void (*DRAW)(WIDGET*,SDL_Surface*),
    uint8_t (*HANDLE_EVENT)(WIDGET*,SDL_Event*)
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
        .DRAW = DRAW,
        .HANDLE_EVENT = HANDLE_EVENT
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

EVENT_DATA WIDGET_LIST_HANDLE_EVENT(WIDGET_LIST* list,SDL_Event* e){
    if(!list || !e) return (EVENT_DATA){NULL,0};

    WIDGET* current = list->tail;
    while(current){
        EVENT_DATA data = WIDGET_LIST_HANDLE_EVENT(&current->children,e);
        if(data.widget) return data;

        if(current->HANDLE_EVENT){
            uint8_t flags = current->HANDLE_EVENT(current,e);
            if(flags & EVENT_HANDLED){
                return (EVENT_DATA){current,flags};
            }
        }

        current = current->prev;
    }

    return (EVENT_DATA){NULL,0};
}

WIDGET_MANAGER* CREATE_WIDGET_MANAGER(){
    return calloc(1,sizeof(WIDGET_MANAGER));
}

void DESTROY_WIDGET_MANAGER(WIDGET_MANAGER* manager){
    if(!manager) return;
    WIDGET_LIST_FREE_WIDGETS(&manager->root);
    free(manager);
}

void WIDGET_MANAGER_DRAW_WIDGETS(WIDGET_MANAGER* manager){
    if(!manager || !manager->surface) return;
    SDL_FillRect(manager->surface, NULL, 0xFFFFFFFF);
    WIDGET_LIST_DRAW_WIDGETS(&manager->root,manager->surface);
}

void REDRAW_WIDGET(WIDGET* w,WIDGET_MANAGER* manager){
    if(!w || !manager->surface) return;
    if(w->DRAW) w->DRAW(w,manager->surface);
    WIDGET_LIST_DRAW_WIDGETS(&w->children,manager->surface);
}

void WIDGET_MANAGER_HANDLE_EVENT(WIDGET_MANAGER* manager,SDL_Event* e){
    if(!manager || !e) return;
    EVENT_DATA e_data = WIDGET_LIST_HANDLE_EVENT(&manager->root,e);
    WIDGET* w = e_data.widget;

    switch(e->type){
        case SDL_MOUSEMOTION:
            if(w!=manager->last_hovered){
                if(manager->last_hovered){
                    manager->last_hovered->hovered = false;
                    REDRAW_WIDGET(manager->last_hovered,manager);
                }
                manager->last_hovered = w;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(e->button.button==SDL_BUTTON_LEFT){
                 if(w && w->dragging) manager->dragging_widget = w;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if(e->button.button==SDL_BUTTON_LEFT){
                if(manager->dragging_widget){
                    manager->dragging_widget->dragging = false;
                    manager->dragging_widget = NULL;
                }
            }
            break;
    }

    if(e_data.flags & REDRAW){
        REDRAW_WIDGET(w,manager);
    }else if(e_data.flags & REDRAW_EVERYTHING){
        WIDGET_MANAGER_DRAW_WIDGETS(manager);
    }
}

WIDGET_LIST* TARGET_ROOT = NULL;
WIDGET* TARGET_WIDGET = NULL;