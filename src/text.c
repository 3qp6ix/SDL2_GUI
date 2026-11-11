#include "text.h"
#include "graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

void UPDATE_TEXT(TEXT* text){
    if(!text || !text->font || !text->buffer) return;
    if(text->font->line_height==0 || text->font_size<text->font->line_height) return;

    FONT_DATA* font = text->font;
    char* buffer = text->buffer;
    SDL_Rect rect = text->rect;

    uint8_t scale = text->font_size/font->line_height;
    uint8_t gap = scale * 2;
    uint8_t line_height = font->line_height * scale;

    int x0 = rect.x;
    int y0 = rect.y;
    int x1 = x0 + rect.w;
    int y1 = y0 + rect.h;

    int cursor_x = x0;

    size_t curr_w = 0;
    size_t w = 0;
    size_t h = line_height;

    while(*buffer){
        char character = (char)toupper(*buffer);

        if(character=='\n'){
            h += line_height + gap;
            cursor_x = x0;
            if(curr_w>w) w=curr_w;
            curr_w = 0;
            buffer++;
            continue;
        }

        GLYPH g = font->glyphs[character];
        if(g.w>0){
            if(text->FLAGS&WRAP){
                if(cursor_x+g.i*scale>x1){
                    cursor_x = x0;
                    h += line_height + gap;

                    if(curr_w>w) w = curr_w;
                    curr_w = 0;
                }
            }
        }

        curr_w += g.i * scale;
        cursor_x += g.i * scale;
        buffer++;
    }

    if(curr_w > w) w = curr_w;

    text->w = w;
    text->h = h - gap;
}

void DESTROY_FONT_DATA(FONT_DATA* f_data){
    if(!f_data) return;

    for(size_t i=0;i<SIZE;i++){
        GLYPH* curr = &f_data->glyphs[i];
        if(curr->h>0){
            free(curr->bitmap);
        }
    }

    free(f_data);
}

FONT_DATA* GET_FONT_DATA(const char* filepath){
    if(!filepath) return NULL;
    FILE* f = fopen(filepath,"rb");
    if(!f) return NULL;

    FONT_DATA* f_data = (FONT_DATA*)calloc(1,sizeof(FONT_DATA));
    if(!f_data){
        fclose(f);
        return NULL;
    }

    fscanf(f,"%zu",&f_data->HOW_MANY);
    fscanf(f,"%zu",&f_data->line_height);

    char token[10];
    for(size_t i=0;i<f_data->HOW_MANY;i++){
        fscanf(f,"%9s",token);

        if(strcmp(token,"SPACE")==0){
            token[0] = ' ';
        }

        GLYPH* g = &f_data->glyphs[token[0]];
        fscanf(f,"%hu,%hu,%hu",&g->w,&g->h,&g->i);

        uint8_t* bitmap = (uint8_t*)calloc(g->h,sizeof(uint8_t));

        if(!bitmap){
            DESTROY_FONT_DATA(f_data);
            fclose(f);
            return NULL;
        }

        char binary[10];
        for(size_t y=0;y<g->h;y++){
            uint8_t bits = 0;
            fscanf(f,"%9s",binary);
            for(size_t x=0;x<g->w;x++){
                bits <<= 1;
                bits |= (binary[x]=='1')? 1 : 0;
            }

            bitmap[y] = bits;
        }

        g->bitmap = bitmap;
    }

    fclose(f);
    return f_data;
}

void DRAW_TEXT(TEXT* text,SDL_Surface* surface){
    if(!text || !surface || !text->font || !text->buffer) return;

    if(text->font->line_height==0 || text->font_size<text->font->line_height) return;

    FONT_DATA* font = text->font;
    char* buffer = text->buffer;
    SDL_Rect rect = text->rect;

    uint8_t scale = text->font_size/font->line_height;
    uint8_t gap = scale * 2;
    uint8_t line_height = font->line_height * scale;

    int x0 = rect.x;
    int y0 = rect.y;
    int x1 = x0 + rect.w;
    int y1 = y0 + rect.h;

    if(text->FLAGS&CENTER_X){
        size_t text_w = text->w;
        if(text_w<rect.w){
            x0 += (rect.w-text_w)/2;
        }
    }

    if(text->FLAGS&CENTER_Y){
        size_t text_h = text->h;
        if(text_h<rect.h){
            y0 += (rect.h-text_h)/2;
        }
    }

    int cursor_x = x0;
    int cursor_y = y0 - text->scroll_y;

    while(*buffer){
        char character = (char)toupper(*buffer);

        if(character=='\n'){
            cursor_x = x0;
            cursor_y += line_height + gap;
            buffer++;
            continue;;
        }

        GLYPH* g = &font->glyphs[character];
        if(g->w>0){
            if(text->FLAGS&WRAP){
                if(cursor_x+g->i*scale>x1){
                    cursor_x = x0;
                    cursor_y += line_height + gap;
                }
            }

            for(size_t y=0;y<g->h;y++){
                if(text->FLAGS&CLIP){
                    if(cursor_y+y*scale<y0) continue;
                    if(cursor_y+y*scale>=y1) continue;
                }

                uint8_t row = g->bitmap[y];
                for(size_t x=0;x<g->w;x++){
                    uint8_t bit = (row>>(g->w-x-1)) & 0b1;
                    if(bit){
                        fill_rect(
                            surface,
                            cursor_x + x * scale,
                            cursor_y + y * scale,
                            scale,scale,text->color
                        );
                    }
                }
            }
        }

        cursor_x += g->i * scale;
        buffer++;
    }
}

FONT_DATA* DEFAULT_FONT = NULL;