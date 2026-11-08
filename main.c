#include "SDL2/SDL.h"
#include "graphics.h"
#include "text.h"
#include <stdio.h>

size_t W_WIDTH = 900;
size_t W_HEIGHT = 600;

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

    bool running = true;
    SDL_Event e;

    SDL_Rect label = {300,150,300,300};
    clear_screen(surface,0xFFFFFFFF);
    fill_rect(surface,label.x,label.y,label.w,label.h,0xff191307);

    TEXT text = (TEXT){
        .buffer = "HELLO WORLD!\n"
        "How are you?\nGood luck:)\n"
        "\n-3qp6ix\n2023 08 11",
        .color = 0xFF00FF00,
        .FLAGS = WRAP | CLIP | CENTER_X | CENTER_Y,
        .font = &default_font,
        .font_size = 16,
        .rect = label
    };

    UPDATE_TEXT(&text);

    DRAW_TEXT(&text,surface);

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
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}