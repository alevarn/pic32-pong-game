#ifndef DISPLAY_H
#define DISPLAY_H

#define VDD_BIT 0x40
#define VBAT_BIT 0x20
#define DATA_CMD_BIT 0x10
#define RESET_BIT 0x200

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 32
#define DISPLAY_PAGES 4

/**
 *  Initierar SPI2 och OLED skärmen.
*/
void init_display(void);

/**
 *  Uppdaterar skärmen utifrån en array.
*/
void update_display(unsigned char *buffer);

#endif