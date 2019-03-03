#ifndef UTILITY_H
#define UTILITY_H

/**
 *  Returnerar längden av en sträng utan '\0'.
 *  Skriven av Jesse Feng.
*/
unsigned int strlen(const char *str);

/**
 *  Konverterar ett heltal till en sträng.
 *  Skriven av Marcus Alevärn.
*/
void int_to_string(unsigned int integer, char *buffer);

/**
 *  Genererar ett pseudo slumptal som inte är större än max.
 *  Skriven av Marcus Alevärn.
*/
unsigned int random(unsigned int max);

/**
 *  Sätter ett initialvärde till metoden random().
 *  Skriven av Marcus Alevärn.
*/
void seed_random(unsigned int seed);

#endif