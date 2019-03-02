#include <pic32mx.h>
#include "../tools/utility.h"
#include "eeprom.h"

void init_eeprom(void)
{
    I2C1STAT = 0;
    I2C1BRG = 0x0C2;
    I2C1CON = PIC32_I2CCON_SIDL | PIC32_I2CCON_ON;
}

// Väntar på att I2C1 är redo.
static void wait()
{
    unsigned char busy = PIC32_I2CCON_SEN | PIC32_I2CCON_RSEN | PIC32_I2CCON_PEN | PIC32_I2CCON_RCEN | PIC32_I2CCON_ACKEN;
    while (I2C1CON & busy || I2C1STAT & PIC32_I2CSTAT_TRSTAT);
}

// Skickar data med I2C1.
static int send(unsigned char data)
{
    wait();
    I2C1TRN = data;
    wait();
    return !(I2C1STAT & PIC32_I2CSTAT_ACKSTAT);
}

// Tar emot data med I2C1.
static unsigned char receive()
{
    wait();
    I2C1CONSET = PIC32_I2CCON_RCEN;
    wait();
    I2C1STATCLR = PIC32_I2CSTAT_I2COV;
    return I2C1RCV;
}

static void acknowledge()
{
    wait();
    I2C1CONCLR = PIC32_I2CCON_ACKDT;
    I2C1CONSET = PIC32_I2CCON_ACKEN;
}

static void not_acknowledge()
{
    wait();
    I2C1CONSET = PIC32_I2CCON_ACKDT;
    I2C1CONSET = PIC32_I2CCON_ACKEN;
}

static void start()
{
    wait();
    I2C1CONSET = PIC32_I2CCON_SEN;
    wait();
}

static void stop()
{
    wait();
    I2C1CONSET = PIC32_I2CCON_PEN;
    wait();
}

// Skickar adressen som ska läsas/skrivas från/till EEPROM minnet.
static void send_address(unsigned short address)
{
    do
    {
        start();
    } while (!send(CONTROL_BYTE_WRITE));

    // Skickar de åtta mest signifikanta bitarna av adressen.
    send(address >> 8);

    // Skickar de åtta minst signifikanta bitarna av adressen.
    send(address);
}

void write_byte_to_eeprom(unsigned short address, unsigned char byte)
{
    send_address(address);
    send(byte);
    stop();
}

void write_int_to_eeprom(unsigned short address, unsigned int integer)
{
    write_byte_to_eeprom(address, integer >> 24);
    write_byte_to_eeprom(address + 1, integer >> 16);
    write_byte_to_eeprom(address + 2, integer >> 8);
    write_byte_to_eeprom(address + 3, integer);
}

void write_string_to_eeprom(unsigned short address, const char *str)
{
    int i = 0;
    while (1)
    {
        // Skickar ett tecken i taget.
        write_byte_to_eeprom(address + i, *str);
        if (*str == '\0')
        {
            return;
        }
        str++;
        i++;
    }
}

unsigned char read_byte_from_eeprom(unsigned short address)
{
    send_address(address);

    do
    {
        start();
    } while (!send(CONTROL_BYTE_READ));

    unsigned char byte = receive();
    not_acknowledge();
    stop();
    return byte;
}

unsigned int read_int_from_eeprom(unsigned short address)
{
    unsigned int integer = 0;
    integer |= read_byte_from_eeprom(address) << 24;
    integer |= read_byte_from_eeprom(address + 1) << 16;
    integer |= read_byte_from_eeprom(address + 2) << 8;
    integer |= read_byte_from_eeprom(address + 3);
    return integer;
}

void read_string_from_eeprom(unsigned short address, char *buffer)
{
    int i = 0;
    while (1)
    {
        // Läser in ett tecken i taget.
        buffer[i] = read_byte_from_eeprom(address + i);
        if (buffer[i] == '\0')
        {
            return;
        }
        i++;
    }
}