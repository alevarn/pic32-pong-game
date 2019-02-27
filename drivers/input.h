#ifndef INPUT_H
#define INPUT_H

#define BTN1_BIT 0x2
#define BTN2_BIT 0x20
#define BTN3_BIT 0x40
#define BTN4_BIT 0x80

void init_input(void);

int btn1_down(void);
int btn2_down(void);
int btn3_down(void);
int btn4_down(void);

int btn1_clicked(void);
int btn2_clicked(void);
int btn3_clicked(void);
int btn4_clicked(void);

#endif