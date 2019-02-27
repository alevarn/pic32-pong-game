#ifndef TIMER_H
#define TIMER_H

#define PERIOD ((80000000 / 64) / 1000)

#if PERIOD > 0xffff
#error "16-bits can't store timer period."
#endif

#define PRESCALE_BITS 0x60
#define START_BIT 0x8000
#define FLAG_BIT 0x100

void init_timer(void);

void delay_ms(unsigned int ms);

#endif