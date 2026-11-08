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
    int cursor_y = y0;

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

static uint8_t default_font_bitmap[SIZE][8] = {
    [65] = {0b00111100,0b01100110,0b11000011,0b11000011,0b11111111,0b11000011,0b11000011,0b11000011}, // A
    [66] = {0b11111100,0b11000110,0b11000110,0b11111100,0b11000110,0b11000110,0b11000110,0b11111100}, // B
    [67] = {0b00111110,0b01100000,0b11000000,0b11000000,0b11000000,0b11000000,0b01100000,0b00111110}, // C
    [68] = {0b11111100,0b11000110,0b11000011,0b11000011,0b11000011,0b11000011,0b11000110,0b11111100}, // D
    [69] = {0b11111111,0b11000000,0b11000000,0b11111100,0b11000000,0b11000000,0b11000000,0b11111111}, // E
    [70] = {0b11111111,0b11000000,0b11000000,0b11111100,0b11000000,0b11000000,0b11000000,0b11000000}, // F
    [71] = {0b00111110,0b01100000,0b11000000,0b11001111,0b11000011,0b11000011,0b01100011,0b00111110}, // G
    [72] = {0b11000011,0b11000011,0b11000011,0b11111111,0b11000011,0b11000011,0b11000011,0b11000011}, // H
    [73] = {0b00111100,0b00011000,0b00011000,0b00011000,0b00011000,0b00011000,0b00011000,0b00111100}, // I
    [74] = {0b00011110,0b00000110,0b00000110,0b00000110,0b00000110,0b11000110,0b11000110,0b01111100}, // J
    [75] = {0b11000110,0b11001100,0b11011000,0b11110000,0b11011000,0b11001100,0b11000110,0b11000110}, // K
    [76] = {0b11000000,0b11000000,0b11000000,0b11000000,0b11000000,0b11000000,0b11000000,0b11111111}, // L
    [77] = {0b11000011,0b11100111,0b11111111,0b11011011,0b11000011,0b11000011,0b11000011,0b11000011}, // M
    [78] = {0b11000011,0b11100011,0b11110011,0b11011011,0b11001111,0b11000111,0b11000011,0b11000011}, // N
    [79] = {0b00111100,0b01100110,0b11000011,0b11000011,0b11000011,0b11000011,0b01100110,0b00111100}, // O
    [80] = {0b11111100,0b11000110,0b11000110,0b11111100,0b11000000,0b11000000,0b11000000,0b11000000}, // P
    [81] = {0b00111100,0b01100110,0b11000011,0b11000011,0b11000011,0b11011011,0b01100110,0b00111101}, // Q
    [82] = {0b11111100,0b11000110,0b11000110,0b11111100,0b11011000,0b11001100,0b11000110,0b11000110}, // R
    [83] = {0b00111110,0b01100000,0b11000000,0b01111100,0b00000110,0b00000011,0b11000110,0b01111100}, // S
    [84] = {0b11111111,0b00011000,0b00011000,0b00011000,0b00011000,0b00011000,0b00011000,0b00011000}, // T
    [85] = {0b11000011,0b11000011,0b11000011,0b11000011,0b11000011,0b11000011,0b01100110,0b00111100}, // U
    [86] = {0b11000011,0b11000011,0b11000011,0b11000011,0b11000011,0b01100110,0b01100110,0b00111100}, // V
    [87] = {0b11000011,0b11000011,0b11000011,0b11000011,0b11011011,0b11111111,0b11100111,0b11000011}, // W
    [88] = {0b11000011,0b11000011,0b01100110,0b00111100,0b00111100,0b01100110,0b11000011,0b11000011}, // X
    [89] = {0b11000011,0b11000011,0b01100110,0b00111100,0b00011000,0b00011000,0b00011000,0b00011000}, // Y
    [90] = {0b11111111,0b00000110,0b00001100,0b00011000,0b00110000,0b01100000,0b11000000,0b11111111}, // Z

    [48] = {0b00111100,0b01100110,0b11000011,0b11000011,0b11000011,0b11000011,0b01100110,0b00111100}, // 0
    [49] = {0b00011000,0b00111000,0b00011000,0b00011000,0b00011000,0b00011000,0b00011000,0b00111100}, // 1
    [50] = {0b00111100,0b01100110,0b00000110,0b00001100,0b00011000,0b00110000,0b01100000,0b01111110}, // 2
    [51] = {0b00111100,0b01100110,0b00000110,0b00011100,0b00000110,0b00000110,0b01100110,0b00111100}, // 3
    [52] = {0b00001100,0b00011100,0b00111100,0b01101100,0b11001100,0b11111110,0b00001100,0b00001100}, // 4
    [53] = {0b01111110,0b01100000,0b01100000,0b01111100,0b00000110,0b00000110,0b01100110,0b00111100}, // 5
    [54] = {0b00111100,0b01100000,0b11000000,0b11111100,0b11000110,0b11000110,0b11000110,0b00111100}, // 6
    [55] = {0b11111110,0b00000110,0b00001100,0b00011000,0b00110000,0b00110000,0b00110000,0b00110000}, // 7
    [56] = {0b00111100,0b01100110,0b01100110,0b00111100,0b01100110,0b01100110,0b01100110,0b00111100}, // 8
    [57] = {0b00111100,0b01100110,0b01100110,0b00111110,0b00000110,0b00000110,0b01100110,0b00111100}, // 9

    [33] = {0b00011000,0b00011000,0b00011000,0b00011000,0b00011000,0b00000000,0b00011000,0b00011000}, // !
    [63] = {0b00111100,0b01100110,0b00000110,0b00001100,0b00011000,0b00011000,0b00000000,0b00011000}, // ?
    [35] = {0b01100110,0b01100110,0b11111111,0b01100110,0b11111111,0b01100110,0b01100110,0b00000000}, // #
    [43] = {0b00011000,0b00011000,0b00011000,0b11111111,0b11111111,0b00011000,0b00011000,0b00000000}, // +
    [45] = {0b00000000,0b00000000,0b00000000,0b11111111,0b11111111,0b00000000,0b00000000,0b00000000}, // -
    [95] = {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11111111,0b11111111}, // _
    [32] = {0,0,0,0,0,0,0,0}, // SPACE
    [58] = {0b00000000,0b00011000,0b00011000,0b00000000,0b00011000,0b00011000,0b00000000,0b00000000}, // :
    [40] = {0b00011000,0b00110000,0b01100000,0b01100000,0b01100000,0b01100000,0b00110000,0b00011000}, // (
    [41] = {0b01100000,0b00110000,0b00011000,0b00011000,0b00011000,0b00011000,0b00110000,0b01100000}, // )
    [91] = {0b00111100,0b00110000,0b00110000,0b00110000,0b00110000,0b00110000,0b00110000,0b00111100}, // [
    [93] = {0b00111100,0b00001100,0b00001100,0b00001100,0b00001100,0b00001100,0b00001100,0b00111100}, // ]
    [123] = {0b00001110,0b00011000,0b00011000,0b01110000,0b00011000,0b00011000,0b00011000,0b00001110}, // {
    [125] = {0b01110000,0b00011000,0b00011000,0b00001110,0b00011000,0b00011000,0b00011000,0b01110000}, // }
    [61] = {0b00000000,0b00000000,0b11111111,0b00000000,0b00000000,0b11111111,0b00000000,0b00000000}, // =
    [46] = {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00011000,0b00011000}, // .
    [44] = {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00011000,0b00011000,0b00110000}, // ,
    [39] = {0b00011000,0b00011000,0b00011000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000}, // '
    [34] = {0b01100110,0b01100110,0b01100110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000}, // "
    [47] = {0b00000011,0b00000110,0b00001100,0b00011000,0b00110000,0b01100000,0b11000000,0b10000000}, // /
    [92] = {0b11000000,0b01100000,0b00110000,0b00011000,0b00001100,0b00000110,0b00000011,0b00000001}, // backslash
    [60] = {0b00000110,0b00001100,0b00011000,0b00110000,0b00110000,0b00011000,0b00001100,0b00000110}, // <
    [62] = {0b01100000,0b00110000,0b00011000,0b00001100,0b00001100,0b00011000,0b00110000,0b01100000}  // >
};

#define DEFAULT_GLYPH(c, advance) [c] = {8, 8, advance, default_font_bitmap[c]}

FONT_DATA default_font = {
    .HOW_MANY = 59,
    .line_height = 8,
    .glyphs = {
        DEFAULT_GLYPH(65, 9),  DEFAULT_GLYPH(66, 9),
        DEFAULT_GLYPH(67, 9),  DEFAULT_GLYPH(68, 9),
        DEFAULT_GLYPH(69, 9),  DEFAULT_GLYPH(70, 9),
        DEFAULT_GLYPH(71, 9),  DEFAULT_GLYPH(72, 9),
        DEFAULT_GLYPH(73, 9),  DEFAULT_GLYPH(74, 9),
        DEFAULT_GLYPH(75, 9),  DEFAULT_GLYPH(76, 9),
        DEFAULT_GLYPH(77, 9),  DEFAULT_GLYPH(78, 9),
        DEFAULT_GLYPH(79, 9),  DEFAULT_GLYPH(80, 9),
        DEFAULT_GLYPH(81, 9),  DEFAULT_GLYPH(82, 9),
        DEFAULT_GLYPH(83, 9),  DEFAULT_GLYPH(84, 9),
        DEFAULT_GLYPH(85, 9),  DEFAULT_GLYPH(86, 9),
        DEFAULT_GLYPH(87, 9),  DEFAULT_GLYPH(88, 9),
        DEFAULT_GLYPH(89, 9),  DEFAULT_GLYPH(90, 9),

        DEFAULT_GLYPH(48, 9),  DEFAULT_GLYPH(49, 9),
        DEFAULT_GLYPH(50, 9),  DEFAULT_GLYPH(51, 9),
        DEFAULT_GLYPH(52, 9),  DEFAULT_GLYPH(53, 9),
        DEFAULT_GLYPH(54, 9),  DEFAULT_GLYPH(55, 9),
        DEFAULT_GLYPH(56, 9),  DEFAULT_GLYPH(57, 9),

        DEFAULT_GLYPH(33, 9),  DEFAULT_GLYPH(63, 9),
        DEFAULT_GLYPH(35, 9),  DEFAULT_GLYPH(43, 9),
        DEFAULT_GLYPH(45, 9),  DEFAULT_GLYPH(95, 9),
        DEFAULT_GLYPH(32, 9),  DEFAULT_GLYPH(58, 6),
        DEFAULT_GLYPH(40, 6),  DEFAULT_GLYPH(41, 6),
        DEFAULT_GLYPH(91, 9),  DEFAULT_GLYPH(93, 9),
        DEFAULT_GLYPH(123, 9), DEFAULT_GLYPH(125, 9),
        DEFAULT_GLYPH(61, 9),  DEFAULT_GLYPH(46, 9),
        DEFAULT_GLYPH(44, 9),  DEFAULT_GLYPH(39, 9),
        DEFAULT_GLYPH(34, 9),  DEFAULT_GLYPH(47, 9),
        DEFAULT_GLYPH(92, 9),  DEFAULT_GLYPH(60, 9),
        DEFAULT_GLYPH(62, 9)
    }
};

#undef DEFAULT_GLYPH