#include "utility.h"

unsigned int strlen(const char *str)
{
    unsigned int count = 0;
    while (*str != '\0')
    {
        count++;
        str++;
    }
    return count;
}

void int_to_string(unsigned int integer, char *buffer)
{
    unsigned char digits = 0;

    // Specialfall.
    if (integer == 0)
    {
        buffer[digits++] = 0x30; // 0x30 = '0'
    }
    else
    {
        while (integer > 0)
        {
            unsigned char remainder = integer % 10;
            integer /= 10;
            buffer[digits++] = remainder + 0x30;    // + 0x30 för att få ASCII siffrorna.
        }
    }

    if (digits > 1)
    {
        // Vänd på strängen.
        unsigned char high = digits - 1;
        unsigned char low = 0;
        while (low < high)
        {
            char temp = buffer[low];
            buffer[low] = buffer[high];
            buffer[high] = temp;
            high--;
            low++;
        }
    }

    buffer[digits] = '\0';
}

static unsigned int next = 123456789;

unsigned int random(unsigned int max)
{
    next = next * 1103515245 + 12345;
    return ((next / 65536) % max);
}

void seed_random(unsigned int seed)
{
    next = seed;
}