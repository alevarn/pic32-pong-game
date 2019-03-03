#ifndef INPUT_H
#define INPUT_H

#define BTN1_BIT 0x2
#define BTN2_BIT 0x20
#define BTN3_BIT 0x40
#define BTN4_BIT 0x80

/**
 *  Initierar TRISD och TRISF registerna för att aktivera knapparna.
 *  Skriven av Marcus Alevärn och Jesse Feng.
*/
void init_input(void);

/**
 *  Returnerar 1 om BTN1 är nertryckt annars 0.
 *  Skriven av Marcus Alevärn och Jesse Feng.
*/
int btn1_down(void);

/**
 *  Returnerar 1 om BTN2 är nertryckt annars 0.
 *  Skriven av Marcus Alevärn och Jesse Feng.
*/
int btn2_down(void);

/**
 *  Returnerar 1 om BTN3 är nertryckt annars 0.
 *  Skriven av Marcus Alevärn och Jesse Feng.
*/
int btn3_down(void);

/**
 *  Returnerar 1 om BTN4 är nertryckt annars 0.
 *  Skriven av Marcus Alevärn och Jesse Feng.
*/
int btn4_down(void);

/**
 *  Returnerar 1 om BTN1 har blivit klickad på d.v.s. ifall
 *  knappen var uppe och sedan blev nertryckt, annars returnerar metoden 0.
 *  Skriven av Marcus Alevärn och Jesse Feng.
*/
int btn1_clicked(void);

/**
 *  Returnerar 1 om BTN2 har blivit klickad på d.v.s. ifall
 *  knappen var uppe och sedan blev nertryckt, annars returnerar metoden 0.
 *  Skriven av Marcus Alevärn och Jesse Feng.
*/
int btn2_clicked(void);

/**
 *  Returnerar 1 om BTN3 har blivit klickad på d.v.s. ifall
 *  knappen var uppe och sedan blev nertryckt, annars returnerar metoden 0.
 *  Skriven av Marcus Alevärn och Jesse Feng.
*/
int btn3_clicked(void);

/**
 *  Returnerar 1 om BTN4 har blivit klickad på d.v.s. ifall
 *  knappen var uppe och sedan blev nertryckt, annars returnerar metoden 0.
 *  Skriven av Marcus Alevärn och Jesse Feng.
*/
int btn4_clicked(void);

#endif