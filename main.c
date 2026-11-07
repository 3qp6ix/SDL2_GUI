#include "SDL2/SDL.h"
#include "graphics.h"
#include "wnd.h"

size_t W_WIDTH = 900;
size_t W_HEIGHT = 800;
int main(int argc,char** argv){
    if(SDL_Init(SDL_INIT_VIDEO)!=0) return 1;

    SDL_Window* window = SDL_CreateWindow(
        "Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        W_WIDTH,W_HEIGHT,
        0
    );

    if(!window) goto quit;

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    if(!surface) goto quit;

    WIDGET_MANAGER* widget_manager = CREATE_WIDGET_MANAGER();
    widget_manager->surface = surface;
    TARGET_ROOT = &widget_manager->root;

    WND* wnd = CREATE_WND(100,100,400,300);

    bool running = true;
    SDL_Event e;

    clear_screen(surface,0xFFFFFFFF);
    WIDGET_LIST_DRAW_WIDGETS(TARGET_ROOT,surface);
    SDL_UpdateWindowSurface(window);

    while(running){
        while(SDL_PollEvent(&e)){
            switch(e.type){
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }


    }

quit:
    DESTROY_WIDGET_MANAGER(widget_manager);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}