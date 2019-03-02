#ifndef UTILITY_H
#define UTILITY_H

unsigned int strlen(const char *str);

void int_to_string(unsigned int integer, char *buffer);

unsigned int random(unsigned int min, unsigned int max);

void seed_random(unsigned int seed);

#endif