#include <pic32mx.h>
#include "drivers/input.h"
#include "drivers/timer.h"
#include "drivers/graphics.h"
#include "drivers/eeprom.h"
#include "tools/utility.h"

#define PLAYER_HEIGHT 6
#define PLAYER_WIDTH 2
#define PLAYER_NAME_LENGTH 10
#define BALL_SIZE 3
#define REFRESH_DELAY 30
#define HIGHSCORE_TABLE_SIZE 9
#define GAME_TIME (1000 * 60 * 2)

typedef enum
{
    EASY,
    MEDIUM,
    HARD,
    IMPOSSIBLE
} Difficulty;

typedef enum
{
    SINGLE_PLAYER,
    MULTIPLAYER,
    HIGHSCORE,
} Selection;

typedef struct
{
    int x;
    int y;
    char name[PLAYER_NAME_LENGTH + 1];
    int score;
} Player;

typedef struct
{
    int x;
    int y;
    int velocityX;
    int velocityY;
} Ball;

/**
 *  Läser ett initialvärde från EEPROM och skriver dit ett nytt värde till EEPROM.
 *  Används för att generera olika slumptal som skiljer sig åt varje gång datorn startas om.
 *  Skriven av Marcus Alevärn.
*/
void fix_seed(void);

/**
 *  Läser in highscores från EEPROM till programmet. Anropas varje gång programmet startas om.
 *  Skriven av Jesse Feng.
*/
void load_highscores(void);

/**
 *  Återställer highscores bitarna i EEPROM till standard värden vilket är "EMPTY" och 0.
 *  Skriven av Jesse Feng.
*/
void reset_highscores(void);

/**
 *  En meny som låter användaren välja "SINGLE PLAYER", "MULTIPLAYER" eller "HIGHSCORE" och returnerar det val som gjorts.
 *  Skriven av Jesse Feng.
*/
Selection main_menu(void);

/**
 *  Den här metoden anropas när användaren väljer att spela "SINGLE PLAYER".
 *  Skriven av Marcus Alevärn och Jesse Feng.
*/
void single_player(void);

/**
 *  Den här metoden anropas när användaren väljer att spela "MULTIPLAYER".
 *  Skriven av Marcus Alevärn och Jesse Feng.
*/
void multiplayer(void);

/**
 *  Den här metoden anropas när användaren väljer "HIGHSCORE".
 *  Skriven av Marcus Alevärn.
*/
void highscore(void);

/**
 *  Tar in nya resultat från ett antal spelare (en eller två) och sorterar om vektorn highscores efter poäng.
 *  Sedan skriver metoden in den nya highscore listan i EEPROM.
 *  Skriven av Marcus Alevärn.
*/
void update_highscore(Player *players, unsigned char numberOfPlayers);

/**
 *  En meny som låter användaren välja "EASY", "MEDIUM", "HARD" eller "IMPOSSIBLE" och returnerar det val som gjorts.
 *  Skriven av Jesse Feng.
*/
Difficulty difficulty_menu(void);

/**
 *  Låter användaren mata in ett namn. Här kan man också bestämma titeln för skärmen (pageTitle).
 *  Skriven av Marcus Alevärn.
*/
void input_name(char *buffer, char *pageTitle);

/**
 *  Flyttar en spelare uppåt 1 pixel och kontrollerar kollision med taket.
 *  Skriven av Jesse Feng.
*/
void move_player_up(Player *player);

/**
 *  Flyttar en spelare neråt 1 pixel och kontrollerar kollision med golvet.
 *  Skriven av Jesse Feng.
*/
void move_player_down(Player *player);

/**
 *  Tar in en spelare som AI:n får kontrollera, beroende på svårighetsgrad så styr den spelaren olika bra.
 *  Skriven av Marcus Alevärn.
*/
void ai_control(Player *player, Ball *ball, Difficulty difficulty);

/**
 *  Återställer positionerna för spelarna och bollen. Den här metoden anropas efter att en spelare gjort poäng och i början av ett nytt spel.
 *  Skriven av Jesse Feng.
*/
void reset_positions(Player *player1, Player *player2, Ball *ball);

/**
 *  Ritar spelarna, bollen, klockan som tickar neråt och den prickade linjen längs mitten av spelplanen.
 *  Skriven av Jesse Feng.
*/
void draw_game(const Player *player1, const Player *player2, const Ball *ball, const int *gameTime);

/**
 *  Uppdaterar bollens position och kontrollerar kollision med väggarna och spelarna och undersöker ifall någon spelare ska få poäng.
 *  Skriven av Marcus Alevärn.
*/
void update_game(Player *player1, Player *player2, Ball *ball, int *gameTime);

static Player highscores[HIGHSCORE_TABLE_SIZE];
static int reset = 1;

int main(void)
{
    // Initierar drivrutinerna.
    init_input();
    init_timer();
    init_display();
    init_eeprom();

    // Laddningsskärm.
    clear_display();
    draw_string("LOADING...", 2, DISPLAY_WIDTH / 2 - get_string_width("LOADING...", 2) / 2, DISPLAY_HEIGHT / 2 - FONT_HEIGHT / 2);
    refresh_display();

    fix_seed();

    load_highscores();

    while (1)
    {
        Selection selection = main_menu();

        switch (selection)
        {
        case SINGLE_PLAYER:
            single_player();
            break;
        case MULTIPLAYER:
            multiplayer();
            break;
        case HIGHSCORE:
            highscore();
            break;
        }
    }
}

void fix_seed(void)
{
    // Lagrat initialvärde.
    unsigned int seed = read_int_from_eeprom(0);

    // Vi beräknar ett nytt initialvärde till nästa gång datorn startas om,
    // vi vill inte använda samma eftersom då kommer mönster att uppreppas.
    unsigned int nextSeed = 0;

    seed_random(seed);

    int i;
    for (i = 0; i < 1000000; i++)
    {
        nextSeed = random(32767);
    }

    // Skriver initialvärdet till EEEPROM.
    write_int_to_eeprom(0, nextSeed);

    seed_random(seed);
}

void load_highscores(void)
{
    unsigned char i;
    unsigned short address = 4;

    for (i = 0; i < HIGHSCORE_TABLE_SIZE; i++)
    {
        read_string_from_eeprom(address, highscores[i].name);
        address += strlen(highscores[i].name) + 1; // +1 eftersom '\0' också lagras i EEPROM.
        highscores[i].score = read_int_from_eeprom(address);
        address += 4;
    }
}

void reset_highscores(void)
{
    unsigned char i;
    unsigned short address = 4;

    for (i = 0; i < HIGHSCORE_TABLE_SIZE; i++)
    {
        write_string_to_eeprom(address, "EMPTY");
        address += strlen("EMPTY") + 1; // +1 eftersom '\0' också lagras i EEPROM.
        write_int_to_eeprom(address, 0);
        address += 4;
    }
}

Selection main_menu(void)
{
    // Håller koll på vilket alternativ som är markerat.
    Selection marked = SINGLE_PLAYER;

    while (1)
    {
        delay_ms(REFRESH_DELAY);
        clear_display();
        draw_string("MAIN MENU", 2, DISPLAY_WIDTH / 2 - get_string_width("MAIN MENU", 2) / 2, 1);
        draw_dotted_horizontal_line(0, 8, DISPLAY_WIDTH - 1, 2, 1);
        draw_string("SINGLE PLAYER", 1, 0, 14);
        draw_string("MULTIPLAYER", 1, 0, 25);
        draw_string("HIGHSCORE", 1, DISPLAY_WIDTH - get_string_width("HIGHSCORE", 1), 14);

        // Ritar ut linjen under det alternativ som är markerat.
        switch (marked)
        {
        case SINGLE_PLAYER:
            draw_horizontal_line(0, 20, get_string_width("SINGLE PLAYER", 1) - 1);
            break;
        case MULTIPLAYER:
            draw_horizontal_line(0, 31, get_string_width("MULTIPLAYER", 1) - 1);
            break;
        case HIGHSCORE:
            draw_horizontal_line(DISPLAY_WIDTH - get_string_width("HIGHSCORE", 1), 20, DISPLAY_WIDTH - 1);
            break;
        }

        refresh_display();

        // Gå uppåt i menyn.
        if (btn4_clicked())
        {
            if (marked != SINGLE_PLAYER)
            {
                marked--;
            }
        }

        // Gå neråt i menyn.
        if (btn3_clicked())
        {
            if (marked != HIGHSCORE)
            {
                marked++;
            }
        }

        // Kontrollera om ett alternativ har blivit valt.
        if (btn2_clicked())
        {
            // Returnera det alternativ som blivit valt.
            return marked;
        }
    }
}

void single_player(void)
{
    Player player1;
    Player bot = {.name = "BOT"};
    Ball ball;

    // Läser in svårighetsgrad.
    Difficulty difficulty = difficulty_menu();

    // Läser in ett valfritt namn på spelaren.
    input_name(player1.name, "ENTER YOUR NAME");
    reset_positions(&player1, &bot, &ball);

    // Sätter startvärden.
    int gameTime = GAME_TIME;
    player1.score = 0;
    bot.score = 0;
    reset = 1;

    while (1)
    {
        delay_ms(REFRESH_DELAY);
        clear_display();
        draw_game(&player1, &bot, &ball, &gameTime);
        refresh_display();

        ai_control(&bot, &ball, difficulty);
        update_game(&player1, &bot, &ball, &gameTime);

        // Undersöker knapparna som ska kontrollera spelare 1.
        if (btn4_down())
        {
            move_player_up(&player1);
        }
        if (btn3_down())
        {
            move_player_down(&player1);
        }

        // Kontrollerar ifall tiden är slut.
        if (gameTime < 1000)
        {
            clear_display();

            if (player1.score > bot.score)
            {
                draw_string(player1.name, 2, DISPLAY_WIDTH / 2 - get_string_width(player1.name, 2) / 2, 1);
                draw_string("WON!", 1, DISPLAY_WIDTH / 2 - get_string_width("WON!", 1) / 2, 8);
            }
            else if (player1.score < bot.score)
            {
                draw_string(bot.name, 2, DISPLAY_WIDTH / 2 - get_string_width(bot.name, 2) / 2, 1);
                draw_string("WON!", 1, DISPLAY_WIDTH / 2 - get_string_width("WON!", 1) / 2, 8);
            }
            else
            {
                draw_string("GAME ENDS", 1, DISPLAY_WIDTH / 2 - get_string_width("GAME ENDS", 1) / 2, 1);
                draw_string("IN A DRAW!", 1, DISPLAY_WIDTH / 2 - get_string_width("IN A DRAW!", 1) / 2, 8);
            }

            refresh_display();

            delay_ms(1000);

            while (!(btn1_clicked() || btn2_clicked() || btn3_clicked() || btn4_clicked()));

            update_highscore(&player1, 1);

            return;
        }
    }
}

void multiplayer(void)
{
    Player player1;
    Player player2;
    Ball ball;

    // Läser in ett namn för spelare 1 och ett till namn för spelare 2.
    input_name(player1.name, "ENTER NAME FOR PLAYER 1");
    input_name(player2.name, "ENTER NAME FOR PLAYER 2");

    reset_positions(&player1, &player2, &ball);

    // Sätter startvärden.
    int gameTime = GAME_TIME;
    player1.score = 0;
    player2.score = 0;
    reset = 1;

    while (1)
    {
        delay_ms(REFRESH_DELAY);
        clear_display();
        draw_game(&player1, &player2, &ball, &gameTime);
        refresh_display();

        update_game(&player1, &player2, &ball, &gameTime);

        // Undersöker knapparna som ska kontrollera spelare 1.
        if (btn4_down())
        {
            move_player_up(&player1);
        }
        if (btn3_down())
        {
            move_player_down(&player1);
        }

        // Undersöker knapparna som ska kontrollera spelare 2.
        if (btn2_down())
        {
            move_player_up(&player2);
        }
        if (btn1_down())
        {
            move_player_down(&player2);
        }

        // Kontrollerar ifall tiden är slut.
        if (gameTime < 1000)
        {
            clear_display();

            if (player1.score > player2.score)
            {
                draw_string(player1.name, 2, DISPLAY_WIDTH / 2 - get_string_width(player1.name, 2) / 2, 1);
                draw_string("WON!", 1, DISPLAY_WIDTH / 2 - get_string_width("WON!", 1) / 2, 8);
            }
            else if (player1.score < player2.score)
            {
                draw_string(player2.name, 2, DISPLAY_WIDTH / 2 - get_string_width(player2.name, 2) / 2, 1);
                draw_string("WON!", 1, DISPLAY_WIDTH / 2 - get_string_width("WON!", 1) / 2, 8);
            }
            else
            {
                draw_string("GAME ENDS", 1, DISPLAY_WIDTH / 2 - get_string_width("GAME ENDS", 1) / 2, 1);
                draw_string("IN A DRAW!", 1, DISPLAY_WIDTH / 2 - get_string_width("IN A DRAW!", 1) / 2, 8);
            }

            refresh_display();

            delay_ms(1000);

            while (!(btn1_clicked() || btn2_clicked() || btn3_clicked() || btn4_clicked()));

            Player players[2] = {player1, player2};

            update_highscore(players, 2);

            return;
        }
    }
}

void highscore(void)
{
    // Ett värde som förskjuter alla object i y-led, detta skapar en "scroll" effekt.
    int y = 0;

    while (1)
    {
        delay_ms(REFRESH_DELAY);
        clear_display();

        draw_string("HIGHSCORE", 2, DISPLAY_WIDTH / 2 - get_string_width("HIGHSCORE", 2) / 2, 1 + y);
        draw_dotted_horizontal_line(0, 8 + y, DISPLAY_WIDTH - 1, 2, 1);

        int i;
        for (i = 0; i < HIGHSCORE_TABLE_SIZE; i++)
        {
            // Skriver ut index siffra, punkt och namn på spelaren och motsvarande poäng.
            draw_int(i + 1, 0, 1, 12 + (12 * i) + y);
            draw_char('.', 6, 12 + (12 * i) + y);
            draw_string(highscores[i].name, 1, 12, 12 + (12 * i) + y);
            draw_int(highscores[i].score, 1, get_string_width(highscores[i].name, 1) + 18, 12 + (12 * i) + y);
        }

        refresh_display();

        if (btn4_clicked())
        {
            if (y != 0)
            {
                // Scrollar uppåt.
                y += 8;
            }
        }

        if (btn3_clicked())
        {
            if (y != -88)
            {
                // Scrollar neråt.
                y -= 8;
            }
        }

        if (btn2_clicked())
        {
            // Går tillbaka till "MAIN MENU".
            return;
        }
    }
}

// Den här metoden liknar metoden main_menu().
Difficulty difficulty_menu(void)
{
    Difficulty marked = EASY;

    while (1)
    {
        delay_ms(REFRESH_DELAY);
        clear_display();
        draw_string("DIFFICULTY", 2, DISPLAY_WIDTH / 2 - get_string_width("DIFFICULTY", 2) / 2, 1);
        draw_dotted_horizontal_line(0, 8, DISPLAY_WIDTH - 1, 2, 1);
        draw_string("EASY", 1, 0, 14);
        draw_string("MEDIUM", 1, 0, 25);
        draw_string("HARD", 1, DISPLAY_WIDTH - get_string_width("HARD", 1), 14);
        draw_string("IMPOSSIBLE", 1, DISPLAY_WIDTH - get_string_width("IMPOSSIBLE", 1), 25);

        switch (marked)
        {
        case EASY:
            draw_horizontal_line(0, 20, get_string_width("EASY", 1) - 1);
            break;
        case MEDIUM:
            draw_horizontal_line(0, 31, get_string_width("MEDIUM", 1) - 1);
            break;
        case HARD:
            draw_horizontal_line(DISPLAY_WIDTH - get_string_width("HARD", 1), 20, DISPLAY_WIDTH - 1);
            break;
        case IMPOSSIBLE:
            draw_horizontal_line(DISPLAY_WIDTH - get_string_width("IMPOSSIBLE", 1), 31, DISPLAY_WIDTH - 1);
            break;
        }

        refresh_display();

        if (btn4_clicked())
        {
            if (marked != EASY)
            {
                marked--;
            }
        }

        if (btn3_clicked())
        {
            if (marked != IMPOSSIBLE)
            {
                marked++;
            }
        }

        if (btn2_clicked())
        {
            return marked;
        }
    }
}

void input_name(char *buffer, char *pageTitle)
{
    char marked = 0x41;
    unsigned char digits = 0;

    // Sätter alla tecken i strängen till '-'.
    int i;
    for (i = 0; i < PLAYER_NAME_LENGTH; i++)
    {
        buffer[i] = 0x2D;
    }

    buffer[PLAYER_NAME_LENGTH] = '\0';

    while (1)
    {
        // Ritar ut all grafik.
        delay_ms(REFRESH_DELAY);
        clear_display();
        draw_string(pageTitle, 1, DISPLAY_WIDTH / 2 - get_string_width(pageTitle, 1) / 2, 1);
        draw_dotted_horizontal_line(0, 8, DISPLAY_WIDTH - 1, 2, 1);
        draw_char(marked, 10, 20);
        draw_rectangle(4, 15, 16, 15);
        draw_string(buffer, 2, 30, 20);
        refresh_display();

        // Kontrollerar om markerad bokstav ska bytas.
        if (btn4_clicked())
        {
            if (marked == 0x41)
            {
                marked = 0x5A;
            }
            else
            {
                marked--;
            }
        }

        // Kontrollerar om markerad bokstav ska bytas.
        if (btn3_clicked())
        {
            if (marked == 0x5A)
            {
                marked = 0x41;
            }
            else
            {
                marked++;
            }
        }

        if (btn2_clicked())
        {
            // Den markerade bokstaven sätts in i strängen.
            buffer[digits++] = marked;
        }

        if (btn1_clicked() || digits == 10)
        {
            // Vi är klara med inmatning av namn och hoppar ur den här metoden.
            buffer[digits] = '\0';
            return;
        }
    }
}

void move_player_up(Player *player)
{
    // Flyttar spelaren uppåt så länge den inte nuddar taket.
    if (player->y != 0)
    {
        player->y--;
    }
}
void move_player_down(Player *player)
{
    // Flyttar spelaren neråt så länge den inte nuddar golvet.
    if (player->y + PLAYER_HEIGHT != DISPLAY_HEIGHT)
    {
        player->y++;
    }
}

void ai_control(Player *player, Ball *ball, Difficulty difficulty)
{
    // Desto högre värde på actionPoints desto bättre drag får AI:n göra.
    static int actionPoints = 0;

    // Antalet iterationer som AI:n kommer använda sig av samma actionPoints värde.
    static int rounds = 0;

    if (rounds == 0)
    {
        // Uppdaterar action points till ett nytt slumpvärde och sätter in nya rounds.
        // Vi tar hänsyn till svårighetsgrad och ser till att generera större slumptal beroende på difficulty.
        actionPoints = random(1000 + (1500 << difficulty));
        rounds = 50;
    }

    // Följer bollen i y-led (bästa möjliga drag).
    if (actionPoints > 1000 && ball->velocityX == 1)
    {
        if (ball->velocityY == 1)
        {
            move_player_down(player);
        }
        else
        {
            move_player_up(player);
        }
        rounds--;
    }
    // Väntar och står helt stilla (normal bra drag).
    else if (actionPoints >= 500 && actionPoints <= 1000 && ball->velocityX == 1)
    {
        rounds--;
    }
    // Går åt motsatt håll gentemot bollen (dåligt drag).
    else if (ball->velocityX == 1)
    {
        if (ball->velocityY == 1)
        {
            move_player_up(player);
        }
        else
        {
            move_player_down(player);
        }

        rounds--;
    }
}

void reset_positions(Player *player1, Player *player2, Ball *ball)
{
    // Sätter startvärden för spelarna och bollen.
    player1->x = 2;
    player1->y = DISPLAY_HEIGHT / 2 - PLAYER_HEIGHT / 2;
    player2->x = DISPLAY_WIDTH - PLAYER_WIDTH - 2;
    player2->y = DISPLAY_HEIGHT / 2 - PLAYER_HEIGHT / 2;
    ball->x = DISPLAY_WIDTH / 2 - BALL_SIZE / 2;
    ball->y = DISPLAY_HEIGHT / 2 - BALL_SIZE / 2;
}

void draw_game(const Player *player1, const Player *player2, const Ball *ball, const int *gameTime)
{
    // Ritar ut spelarna och bollen.
    draw_filled_rectangle(player1->x, player1->y, PLAYER_WIDTH, PLAYER_HEIGHT);
    draw_filled_rectangle(player2->x, player2->y, PLAYER_WIDTH, PLAYER_HEIGHT);
    draw_filled_rectangle(ball->x, ball->y, BALL_SIZE, BALL_SIZE);
    draw_dotted_vertical_line(DISPLAY_WIDTH / 2, 0, 26, 4, 3);

    // Ritar ut poängen för spelarna.
    draw_int(player1->score, 1, DISPLAY_WIDTH / 2 - get_int_width(player1->score, 1) - 16, 0);
    draw_int(player2->score, 1, DISPLAY_WIDTH / 2 + get_int_width(player2->score, 1) / 2 + 15, 0);

    unsigned int minutes = (*gameTime) / 1000 / 60;
    unsigned int seconds = (*gameTime) / 1000 % 60;
    draw_int(minutes / 10, 0, 51, 27); // Första minut-siffran.
    draw_int(minutes % 10, 0, 57, 27); // Andra minut-siffran.
    draw_char(':', 64, 27);
    draw_int(seconds / 10, 0, 68, 27); // Första sekund-siffran.
    draw_int(seconds % 10, 0, 74, 27); // Andra sekund-siffran.
}

void update_game(Player *player1, Player *player2, Ball *ball, int *gameTime)
{
    // Om reset = 1 så ska spelet stå stilla och bollen ska inte röra på sig.
    if (reset)
    {
        // Vi återställer positioner.
        reset_positions(player1, player2, ball);
        clear_display();
        draw_game(player1, player2, ball, gameTime);
        refresh_display();

        // Beräknar en slumpmässig riktning åt bollen. Det skulle
        // vara tråkigt om bollen började röra sig åt samma håll varje gång spelet började.
        int ballRandomStart = random(4);

        // Det finns totalt 4 olika kombinationer.
        switch (ballRandomStart)
        {
        case 0:
            ball->velocityX = 1;
            ball->velocityY = 1;
            break;
        case 1:
            ball->velocityX = -1;
            ball->velocityY = 1;
            break;
        case 2:
            ball->velocityX = 1;
            ball->velocityY = -1;
            break;
        case 3:
            ball->velocityX = -1;
            ball->velocityY = -1;
            break;
        }

        // Väntar på ett klick från vilken knapp som helst, när ett klick har registrerats då börjar spelet igen.
        while (!(btn1_clicked() || btn2_clicked() || btn3_clicked() || btn4_clicked()));

        reset = 0;
    }
    else
    {
        // Minskar på tiden som är kvar av spelet.
        *gameTime -= REFRESH_DELAY;

        // Kontrollerar kollision mellan bollen, taket och golvet.
        if (ball->y == 0 || ball->y + BALL_SIZE == DISPLAY_HEIGHT)
        {
            // Ser till att bollen byter riktning i y-led.
            ball->velocityY *= -1;
        }

        // Kontrollerar kollision med spelare 1.
        if (ball->x == player1->x + PLAYER_WIDTH && ball->y + BALL_SIZE >= player1->y && ball->y <= player1->y + PLAYER_HEIGHT)
        {
            ball->velocityX *= -1;
        }

        // Kontrollerar kollision med spelare 2.
        if (ball->x + BALL_SIZE == player2->x && ball->y + BALL_SIZE >= player2->y && ball->y <= player2->y + PLAYER_HEIGHT)
        {
            ball->velocityX *= -1;
        }

        // Kontrollerar om spelare 1 ska ha ett poäng.
        if (ball->x + BALL_SIZE == DISPLAY_WIDTH)
        {
            player1->score++;
            reset = 1;
        }
        // Kontrollerar om spelare 2 ska ha ett poäng.
        else if (ball->x == 0)
        {
            player2->score++;
            reset = 1;
        }
        else
        {
            // Uppdaterar bollens position.
            ball->x += ball->velocityX;
            ball->y += ball->velocityY;
        }
    }
}

void update_highscore(Player *players, unsigned char numberOfPlayers)
{
    // Vi går igenom alla spelare.
    int playerIndex;
    for (playerIndex = 0; playerIndex < numberOfPlayers; playerIndex++)
    {   
        // Skapar en kopia av highscores listan.
        Player newHighscores[9];

        int recordIndex;
        for (recordIndex = 0; recordIndex < HIGHSCORE_TABLE_SIZE; recordIndex++)
        {
            // Kontrollerar om den nya spelarens poäng ska placeras på en viss position i listan.
            if (players[playerIndex].score >= highscores[recordIndex].score)
            {
                // Vi stoppar in det nya rekordet i listan. 
                newHighscores[recordIndex] = players[playerIndex];
                
                int i;
                for (i = recordIndex; i < HIGHSCORE_TABLE_SIZE - 1; i++)
                {
                    // Vi kopierar över alla kvarstående rekord från den gamla listan in till den nya.
                    newHighscores[i + 1] = highscores[i];
                }
                break;
            }

            // Vi stoppar in alla gamla rekord i den nya listan.
            newHighscores[recordIndex] = highscores[recordIndex];
        }

        // Kopierar över den nya listan till den gamla.
        int j;
        for (j = 0; j < HIGHSCORE_TABLE_SIZE; j++)
        {
            highscores[j] = newHighscores[j];
        }
    }

    unsigned short address = 4;

    int i;
    for (i = 0; i < HIGHSCORE_TABLE_SIZE; i++)
    {
        // Vi skriven information från listan till EEPROM minnet.
        write_string_to_eeprom(address, highscores[i].name);
        address += strlen(highscores[i].name) + 1;
        write_int_to_eeprom(address, highscores[i].score);
        address += 4;
    }
}