#ifndef EEPROM_H
#define EEPROM_H

#define CONTROL_BYTE_WRITE 0xA0
#define CONTROL_BYTE_READ 0xA1

void init_eeprom(void);

void write_byte(unsigned short address, unsigned char byte);

void write_int(unsigned short address, unsigned int integer);

void write_string(unsigned short address, const char *str);

unsigned char read_byte(unsigned short address);

unsigned int read_int(unsigned short address);

unsigned char *read_string(unsigned short address);

#endif