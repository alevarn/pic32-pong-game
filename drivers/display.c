#include <pic32mx.h>
#include "display.h"

static unsigned char spi_transmission(unsigned char data)
{
    while (!(SPI2STAT & PIC32_SPISTAT_SPITBE)); // Vänta tills överföringsbuffert är tom.
    SPI2BUF = data;
    while (!(SPI2STAT & PIC32_SPISTAT_SPIRBF)); // Vänta tills mottagningsbuffert är full.
    return SPI2BUF;
}

void init_display(void)
{
    // Initierar SPI2.
    SPI2CON = 0;
    SPI2BRG = 15;
    SPI2STATCLR = PIC32_SPISTAT_SPIROV;
    SPI2CONSET = PIC32_SPICON_MSTEN;
    SPI2CONSET = PIC32_SPICON_CKP;
    SPI2CONSET = PIC32_SPICON_ON;

    TRISFCLR = VDD_BIT | VBAT_BIT | DATA_CMD_BIT;
    TRISGCLR = RESET_BIT;

    // Vi vill skicka kommandon så vi tar bort data kontroll biten, sedan sätter vi på VDD och väntar lite.
    PORTFCLR = VDD_BIT | DATA_CMD_BIT;
    delay_ms(1);

    spi_transmission(0xAE);

    PORTGCLR = RESET_BIT;
    delay_ms(1);
    PORTGSET = RESET_BIT;


    spi_transmission(0x8D);
    spi_transmission(0x14);
    spi_transmission(0xD9);
    spi_transmission(0xF1);

    PORTFCLR = VBAT_BIT;
    delay_ms(100);

    // Inverterar skärmen och sätter origo i det övre vänstra hörnet.
    spi_transmission(0xA1);
    spi_transmission(0xC8);
    spi_transmission(0xDA);
    spi_transmission(0x20);

    spi_transmission(0xAF);
}

void update_display(unsigned char *buffer)
{
    unsigned char page;
    for (page = 0; page < DISPLAY_PAGES; page++)
    {
        PORTFCLR = DATA_CMD_BIT;

        // Sätt vilken page vi vill skriva till.
        spi_transmission(0x22);
        spi_transmission(page);

        // Börja med den vänstra kolumnen.
        spi_transmission(0x0);
        spi_transmission(0x10);
        PORTFSET = DATA_CMD_BIT;
    
        unsigned char column;
        for (column = 0; column < DISPLAY_WIDTH; column++)
        {
            spi_transmission(*buffer);
            buffer++;
        }
    }
}