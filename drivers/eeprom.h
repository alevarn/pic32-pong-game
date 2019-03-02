#ifndef EEPROM_H
#define EEPROM_H

#define CONTROL_BYTE_WRITE 0xA0
#define CONTROL_BYTE_READ 0xA1

/**
 *  Initierar I2C1 för att kunna kommunicera med EEPROM.
*/
void init_eeprom(void);

/**
 *  Skriven 8-bitar till en angiven adress i EEPROM minnet.
*/
void write_byte_to_eeprom(unsigned short address, unsigned char byte);

/**
 *  Skriven 32-bitar till en angiven adress i EEPROM minnet.
*/
void write_int_to_eeprom(unsigned short address, unsigned int integer);

/**
 *  Skriven en sträng till en angiven adress i EEPROM minnet.
*/
void write_string_to_eeprom(unsigned short address, const char *str);

/**
 *  Läser 8-bitar från en angiven adress i EEPROM minnet.
*/
unsigned char read_byte_from_eeprom(unsigned short address);

/**
 *  Läser 32-bitar från en angiven adress i EEPROM minnet.
*/
unsigned int read_int_from_eeprom(unsigned short address);

/**
 *  Läser en sträng från en angiven adress i EEPROM minnet.
*/
void read_string_from_eeprom(unsigned short address, char *buffer);

#endif