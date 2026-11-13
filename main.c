#include "SDL2/SDL.h"
#include "graphics.h"
#include "text.h"
#include "wnd.h"
#include "label.h"
#include "text_input.h"

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

    DEFAULT_FONT = GET_FONT_DATA("./assets/default_font.txt");

    WND* wnd = CREATE_WND(100,100,400,300);
    
    TARGET_WIDGET = (WIDGET*)wnd;
    
    WND* wnd_b = CREATE_WND(5,30,200,100);
    wnd_b->background_color = 0xFFE8ECF2;
    wnd_b->tab_color = 0xFFCBD5E1;
    wnd_b->title = "WINDOW B";
    wnd_b->title_color = 0xFF000000;

    LABEL* label_a = CREATE_LABEL(212,30,180,100);
    label_a->title = "HELLO WORLD!";
    label_a->text.buffer = "  #### LABEL ####\n"
    "HELLO, STRANGER.\n\n"
    "HOW ARE YOU?\n\n"
    "GOOD LUCK :) :]";
    label_a->text.font = DEFAULT_FONT;
    UPDATE_TEXT(&label_a->text); // JUST SETS THE TEXT WIDTH AND HEIGHT (its needed for centering text)
    label_a->text.FLAGS = WRAP | CLIP | CENTER_X | CENTER_Y;
    
    bool running = true;
    SDL_Event e;

    clear_screen(surface,0xFFFFFFFF);
    WIDGET_MANAGER_DRAW_WIDGETS(widget_manager);

    while(running){
        while(SDL_PollEvent(&e)){
            WIDGET_MANAGER_HANDLE_EVENT(widget_manager,&e);
            switch(e.type){
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }

        SDL_UpdateWindowSurface(window);
    }

quit:
    DESTROY_FONT_DATA(DEFAULT_FONT);
    DESTROY_WIDGET_MANAGER(widget_manager);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}