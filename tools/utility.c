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

    if (integer == 0)
    {
        buffer[digits++] = 0x30;
    }
    else
    {
        while (integer > 0)
        {
            unsigned char remainder = integer % 10;
            integer /= 10;
            buffer[digits++] = remainder + 0x30;
        }
    }

    if (digits > 1)
    {
        // Reverse the string.
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