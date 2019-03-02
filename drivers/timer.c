#include <pic32mx.h>
#include "timer.h"

void init_timer(void)
{
    T2CON = PRESCALE_BITS;
    PR2 = PERIOD;
    T2CONSET = START_BIT;
}

void delay_ms(unsigned int ms)
{
    unsigned int countMs = 0;
    TMR2 = 0;
    while (countMs < ms)
    {
        // Kontrollerar om en millisekund har gått.
        if (IFS(0) & FLAG_BIT) 
        {
            IFSCLR(0) = FLAG_BIT;
            countMs++;
        }
    }
}
