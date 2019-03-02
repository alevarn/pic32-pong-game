#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "display.h"

#define BUFFER_SIZE (DISPLAY_WIDTH * DISPLAY_PAGES)
#define FONT_WIDTH 5
#define FONT_HEIGHT 5

/**
 *  Aktiverar pixeln på den angivna koordinaten.
*/
void set_pixel(int x, int y);

/**
 *  Avaktiverar pixeln på den angivna koordinaten.
*/
void clear_pixel(int x, int y);

/**
 *  Ritar en vertikal linje vid en angiven x-koordinat som fyller i pixlarna från y till y2.
*/
void draw_vertical_line(int x, int y, int y2);

/**
 *  Ritar en vertikal prickad linje vid en angiven x-koordinat som fyller i pixlarna från y till y2.
 *  Metoden tar in ytterligare två argument som bestämmer längden av det ifyllda (filledSize) och längden av mellanrummet (spaceSize).
*/
void draw_dotted_vertical_line(int x, int y, int y2, unsigned int filledSize, unsigned int spaceSize);

/**
 *  Ritar en horisontell linje vid en angiven y-koordinat som fyller i pixlarna från x till x2.
*/
void draw_horizontal_line(int x, int y, int x2);

/**
 *  Ritar en horisontell prickad linje vid en angiven y-koordinat som fyller i pixlarna från x till x2.
 *  Metoden tar in ytterligare två argument som bestämmer längden av det ifyllda (filledSize) och längden av mellanrummet (spaceSize).
*/
void draw_dotted_horizontal_line(int x, int y, int x2, unsigned int filledSize, unsigned int spaceSize);

/**
 *  Ritar en rektangel vid en angiven koordinat med önskad bredd och höjd. 
*/
void draw_rectangle(int x, int y, unsigned int width, unsigned int height);

/**
 *  Ritar en ifylld rektangel vid en angiven koordinat med önskad bredd och höjd. 
*/
void draw_filled_rectangle(int x, int y, unsigned int width, unsigned int height);

/**
 *  Ritar en sträng vid en angiven koordinat som har ett särskilt mellanrum mellan varje tecken (spacing).
*/
void draw_string(const char *str, unsigned int spacing, int x, int y);

/**
 *  Ritar ett tecken vid en angiven koordinat.
*/
void draw_char(char c, int x, int y);

/**
 * Ritar ett positivt heltal vid en angiven koordinat som har ett särskilt mellanrum mellan varje siffra (spacing).
*/
void draw_int(unsigned int integer, unsigned int spacing, int x, int y);

/**
 *  Returnerar bredden i pixlar för ett visst tecken. 
*/
unsigned int get_char_width(char c);

/**
 *  Returnerar bredden i pixlar för en sträng som har ett särskilt mellanrum mellan varje tecken (spacing).
*/
unsigned int get_string_width(const char *str, unsigned int spacing);

/**
 *  Returnerar bredden i pixlar för ett heltal som har ett särskilt mellanrum mellan varje siffra (spacing).
*/
unsigned int get_int_width(unsigned int integer, unsigned int spacing);

/**
 *  Avaktiverar alla pixlar, med andra ord skärmen blir helt svart.
*/
void clear_display(void);

/**
 *  Uppdaterar skärmen och gör alla förändringar synliga.
 *  Glöm inte bort att anropa den här metoden! Ett anrop till exempelvis set_pixel(x,y)
 *  kommer inte att märkas förens man gjort ett anrop till refresh_display().
*/
void refresh_display(void);

#endif