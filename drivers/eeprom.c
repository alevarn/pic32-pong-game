#include <pic32mx.h>
#include "../tools/utility.h"
#include "eeprom.h"

void init_eeprom(void)
{
    I2C1STAT = 0;
    I2C1BRG = 0x0C2;
    I2C1CON = PIC32_I2CCON_SIDL | PIC32_I2CCON_ON;
}

static void wait()
{
    unsigned char busy = PIC32_I2CCON_SEN | PIC32_I2CCON_RSEN | PIC32_I2CCON_PEN | PIC32_I2CCON_RCEN | PIC32_I2CCON_ACKEN;
    while (I2C1CON & busy || I2C1STAT & PIC32_I2CSTAT_TRSTAT);
}

static int send(unsigned char data)
{
    wait();
    I2C1TRN = data;
    wait();
    return !(I2C1STAT & PIC32_I2CSTAT_ACKSTAT);
}

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

static void send_address(unsigned short address)
{
    do
    {
        start();
    } while (!send(CONTROL_BYTE_WRITE));

    send(address >> 8);
    send(address);
}

void write_byte(unsigned short address, unsigned char byte)
{
    send_address(address);
    send(byte);
    stop();
}

void write_int(unsigned short address, unsigned int integer)
{
    write_byte(address, integer >> 24);
    write_byte(address + 1, integer >> 16);
    write_byte(address + 2, integer >> 8);
    write_byte(address + 3, integer);
}

void write_string(unsigned short address, const char *str, unsigned int maxLength)
{
    int i;
    for (i = 0; i < maxLength; i++)
    {
        write_byte(address + i, *str);
        if (*str == '\0')
        {
            return;
        }
        str++;
    }
}

unsigned char read_byte(unsigned short address)
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

unsigned int read_int(unsigned short address)
{
    unsigned int integer = 0;
    integer |= read_byte(address) << 24;
    integer |= read_byte(address + 1) << 16;
    integer |= read_byte(address + 2) << 8;
    integer |= read_byte(address + 3);
    return integer;
}

void read_string(unsigned short address, char *buffer, unsigned int maxLength)
{
    int i;
    for (i = 0; i < maxLength; i++)
    {
        buffer[i] = read_byte(address + i);
        if (buffer[i] == '\0')
        {
            return;
        }
    }
}