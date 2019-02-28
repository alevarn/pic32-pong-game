#include "utility.h"

unsigned int strlen(char *str)
{
    int count = 0;
    while (*str != '\0')
    {
        count++;
        str++;
    }
    return count;
}

char *int_to_string(unsigned int integer)
{
    static char str[11];
    unsigned char digits = 0;

    if (integer == 0)
    {
        str[digits++] = 0x30;
    }
    else
    {
        while (integer > 0)
        {
            unsigned char remainder = integer % 10;
            integer /= 10;
            str[digits++] = remainder + 0x30;
        }
    }

    if (digits > 1)
    {
        // Reverse the string.
        unsigned char high = digits - 1;
        unsigned char low = 0;
        while (low < high)
        {
            char temp = str[low];
            str[low] = str[high];
            str[high] = temp;
            high--;
            low++;
        }
    }

    str[digits] = '\0';

    return str;
}