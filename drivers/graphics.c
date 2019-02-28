#include <pic32mx.h>
#include "../tools/utility.h"
#include "graphics.h"

static unsigned char buffer[BUFFER_SIZE];

static const unsigned int font[] = {
    0x00000000, // ' '
    0x00100421, // '!'
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x00208422, // '('
    0x00110841, // ')'
    0x0,
    0x0,
    0x0,
    0x0,
    0x00100000, // '.'
    0x0,
    0x0064A526, // '0'
    0x00710862, // '1'
    0x00F09907, // '2'
    0x00741907, // '3'
    0x00843D29, // '4'
    0x00741C2F, // '5'
    0x00649C26, // '6'
    0x0021110F, // '7'
    0x00649926, // '8'
    0x00643926, // '9'
    0x00008020, // ':'
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0094BD26, // 'A'
    0x00749D27, // 'B'
    0x00E0842E, // 'C'
    0x0074A527, // 'D'
    0x00F09C2F, // 'E'
    0x00109C2F, // 'F'
    0x00E4B42E, // 'G'
    0x0094BD29, // 'H'
    0x00710847, // 'I'
    0x0064A10F, // 'J'
    0x00928CA9, // 'K'
    0x00F08421, // 'L'
    0x0118D771, // 'M'
    0x0094B569, // 'N'
    0x0064A526, // 'O'
    0x00109D27, // 'P'
    0x00F6A52F, // 'Q'
    0x00929D27, // 'R'
    0x0074182E, // 'S'
    0x00210847, // 'T'
    0x0064A529, // 'U'
    0x002294A5, // 'V'
    0x00AAD6B1, // 'W'
    0x00949929, // 'X'
    0x002108A5, // 'Y'
    0x00F0990F, // 'Z'
};

static const unsigned char fontWidth[] = {
    4, // ' '
    1, // '!'
    0,
    0,
    0,
    0,
    0,
    0,
    2, // '('
    2, // ')'
    0,
    0,
    0,
    0,
    1, // '.'
    0,
    4, // '0'
    3, // '1'
    4, // '2'
    4, // '3'
    4, // '4'
    4, // '5'
    4, // '6'
    4, // '7'
    4, // '8'
    4, // '9'
    1, // ':'
    0,
    0,
    0,
    0,
    0,
    0,
    4, // 'A'
    4, // 'B'
    4, // 'C'
    4, // 'D'
    4, // 'E'
    4, // 'F'
    4, // 'G'
    4, // 'H'
    3, // 'I'
    4, // 'J'
    4, // 'K'
    4, // 'L'
    5, // 'M'
    4, // 'N'
    4, // 'O'
    4, // 'P'
    4, // 'Q'
    4, // 'R'
    4, // 'S'
    3, // 'T'
    4, // 'U'
    3, // 'V'
    5, // 'W'
    4, // 'X'
    3, // 'Y'
    4, // 'Z'
};

void set_pixel(int x, int y)
{
    if (x < 0 || x > DISPLAY_WIDTH - 1 || y < 0 || y > DISPLAY_HEIGHT - 1)
        return;

    unsigned char page = y / 8;
    unsigned short byte = x + page * DISPLAY_WIDTH;
    unsigned char bit = 1 << (y % 8);
    buffer[byte] |= bit;
}

void clear_pixel(int x, int y)
{
    if (x < 0 || x > DISPLAY_WIDTH - 1 || y < 0 || y > DISPLAY_HEIGHT - 1)
        return;

    unsigned char page = y / 8;
    unsigned short byte = x + page * DISPLAY_WIDTH;
    unsigned char bit = 1 << (y % 8);
    buffer[byte] &= ~bit;
}

void draw_vertical_line(int x, int y, int y2)
{
    int minY = y < y2 ? y : y2;
    int maxY = y > y2 ? y : y2;

    while (minY <= maxY)
    {
        set_pixel(x, minY++);
    }
}

void draw_dotted_vertical_line(int x, int y, int y2, unsigned int filledSize, unsigned int spaceSize)
{
    int minY = y < y2 ? y : y2;
    int maxY = y > y2 ? y : y2;

    unsigned int filledCount = 0;

    while (minY <= maxY)
    {
        if (filledCount == filledSize)
        {
            minY += spaceSize;
            filledCount = 0;
        }
        else
        {
            set_pixel(x, minY++);
            filledCount++;
        }
    }
}

void draw_horizontal_line(int x, int y, int x2)
{
    int minX = x < x2 ? x : x2;
    int maxX = x > x2 ? x : x2;

    while (minX <= maxX)
    {
        set_pixel(minX++, y);
    }
}

void draw_dotted_horizontal_line(int x, int y, int x2, unsigned int filledSize, unsigned int spaceSize)
{
    int minX = x < x2 ? x : x2;
    int maxX = x > x2 ? x : x2;

    unsigned int filledCount = 0;

    while (minX <= maxX)
    {
        if (filledCount == filledSize)
        {
            minX += spaceSize;
            filledCount = 0;
        }
        else
        {
            set_pixel(minX++, y);
            filledCount++;
        }
    }
}

void draw_rectangle(int x, int y, unsigned int width, unsigned int height)
{
    int xCopy, yCopy;
    for (xCopy = x; xCopy < x + width; xCopy++)
    {
        set_pixel(xCopy, y);
        set_pixel(xCopy, y + height - 1);
    }
    for (yCopy = y; yCopy < y + height; yCopy++)
    {
        set_pixel(x, yCopy);
        set_pixel(x + width - 1, yCopy);
    }
}

void draw_filled_rectangle(int x, int y, unsigned int width, unsigned int height)
{
    int xCopy, yCopy;
    for (yCopy = y; yCopy < y + height; yCopy++)
    {
        for (xCopy = x; xCopy < x + width; xCopy++)
        {
            set_pixel(xCopy, yCopy);
        }
    }
}

void draw_string(const char *str, unsigned int spacing, int x, int y)
{
    int xCopy, yCopy;
    while (*str != '\0')
    {
        unsigned char charIndex = *str - 0x20;
        unsigned char bit = 0;
        for (yCopy = y; yCopy < y + 5; yCopy++)
        {
            for (xCopy = x; xCopy < x + 5; xCopy++)
            {
                if (font[charIndex] & (1 << bit))
                {
                    set_pixel(xCopy, yCopy);
                }
                bit++;
            }
        }
        x += fontWidth[charIndex] + spacing;
        str++;
    }
}

void draw_char(char value, int x, int y)
{
    char c[] = {value, '\0'};
    draw_string(c, 0, x, y);
}

void draw_int(unsigned int value, unsigned int spacing, int x, int y)
{
    draw_string(int_to_string(value), spacing, x, y);
}

unsigned int get_char_length(char value)
{
    return fontWidth[value - 0x20];
}

unsigned int get_string_length(const char *str, unsigned int spacing)
{
    unsigned int length = 0;
    while (*str != '\0')
    {
        length += get_char_length(*str) + spacing;
        str++;
    }
    length -= spacing;
    return length;
}

unsigned int get_int_length(unsigned int value, unsigned int spacing)
{
    return get_string_length(int_to_string(value), spacing);
}

void clear_display(void)
{
    short byte;
    for (byte = 0; byte < BUFFER_SIZE; byte++)
    {
        buffer[byte] = 0;
    }
}

void refresh_display(void)
{
    update_display(buffer);
}