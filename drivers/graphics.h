#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "display.h"

#define BUFFER_SIZE (DISPLAY_WIDTH * DISPLAY_PAGES)
#define FONT_WIDTH 5
#define FONT_HEIGHT 5

void set_pixel(int x, int y);

void clear_pixel(int x, int y);

void draw_vertical_line(int x, int y, int y2);

void draw_dotted_vertical_line(int x, int y, int y2, unsigned int filledSize, unsigned int spaceSize);

void draw_horizontal_line(int x, int y, int x2);

void draw_dotted_horizontal_line(int x, int y, int x2, unsigned int filledSize, unsigned int spaceSize);

void draw_rectangle(int x, int y, unsigned int width, unsigned int height);

void draw_filled_rectangle(int x, int y, unsigned int width, unsigned int height);

void draw_string(const char *str, unsigned int spacing, int x, int y);

void draw_char(char c, int x, int y);

void draw_int(unsigned int integer, unsigned int spacing, int x, int y);

unsigned int get_char_width(char c);

unsigned int get_string_width(const char *str, unsigned int spacing);

unsigned int get_int_width(unsigned int integer, unsigned int spacing);

void clear_display(void);

void refresh_display(void);

#endif