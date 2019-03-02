#include <pic32mx.h>
#include "input.h"

void init_input(void)
{
    TRISFSET = BTN1_BIT;
    TRISDSET = BTN2_BIT | BTN3_BIT | BTN4_BIT;
}

int btn1_down(void)
{
    return PORTF & BTN1_BIT;
}

int btn2_down(void)
{
    return PORTD & BTN2_BIT;
}

int btn3_down(void)
{
    return PORTD & BTN3_BIT;
}

int btn4_down(void)
{
    return PORTD & BTN4_BIT;
}

int btn1_clicked(void)
{
    static int previousState = 0;

    if (!previousState)
    {
        if (btn1_down())
        {
            previousState = 1;
            return 1;
        }
    }
    else
    {
        previousState = btn1_down();
        return 0;
    }
}

int btn2_clicked(void)
{
    static int previousState = 0;

    if (!previousState)
    {
        if (btn2_down())
        {
            previousState = 1;
            return 1;
        }
    }
    else
    {
        previousState = btn2_down();
        return 0;
    }
}
int btn3_clicked(void)
{
    static int previousState = 0;

    if (!previousState)
    {
        if (btn3_down())
        {
            previousState = 1;
            return 1;
        }
    }
    else
    {
        previousState = btn3_down();
        return 0;
    }
}

int btn4_clicked(void)
{
    static int previousState = 0;

    if (!previousState)
    {
        if (btn4_down())
        {
            previousState = 1;
            return 1;
        }
    }
    else
    {
        previousState = btn4_down();
        return 0;
    }
}