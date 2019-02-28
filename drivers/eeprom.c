#include <pic32mx.h>
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
    send_address(address);
    send(integer >> 24);
    send(integer >> 16);
    send(integer >> 8);
    send(integer);
    stop();
}

void write_string(unsigned short address, const char *str)
{
    send_address(address);

    while (*str != '\0')
    {
        send(*str);
        str++;
    }

    send('\0');
    stop();
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
    send_address(address);

    do
    {
        start();
    } while (!send(CONTROL_BYTE_READ));

    int integer = 0;

    integer |= receive() << 24;
    acknowledge();
    integer |= receive() << 16;
    acknowledge();
    integer |= receive() << 8;
    acknowledge();
    integer |= receive();

    not_acknowledge();
    stop();
    return integer;
}

unsigned char *read_string(unsigned short address)
{
    send_address(address);

    do
    {
        start();
    } while (!send(CONTROL_BYTE_READ));

    static char str[64];

    int i;
    for (i = 0; i < 64; i++)
    {
        str[i] = receive();
        if (str[i] != '\0')
        {
            acknowledge();
        }
        else
        {
            not_acknowledge();
            stop();
            return str;
        }
    }
}