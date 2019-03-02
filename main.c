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

void fix_seed(void);
void load_highscores(void);
void reset_highscores(void);
Selection main_menu(void);
void single_player(void);
void multiplayer(void);
void highscore(void);
void update_highscore(Player *players, unsigned char numberOfPlayers);
Difficulty difficulty_menu(void);
void input_name(char *buffer, char *pageTitle);
void move_player_up(Player *player);
void move_player_down(Player *player);
void ai_control(Player *player, Ball *ball, Difficulty difficulty);
void reset_positions(Player *player1, Player *player2, Ball *ball);
void draw_game(const Player *player1, const Player *player2, const Ball *ball, const int *gameTime);
void update_game(Player *player1, Player *player2, Ball *ball, int *gameTime);

static Player highscores[HIGHSCORE_TABLE_SIZE];
static int reset = 1;

int main(void)
{
    init_input();
    init_timer();
    init_display();
    init_eeprom();

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
    unsigned int seed = read_int(0);
    unsigned int nextSeed = 0;

    seed_random(seed);

    int i;
    for (i = 0; i < 1000000; i++)
    {
        nextSeed = random(0, 32767);
    }

    write_int(0, nextSeed);

    seed_random(seed);
}

void load_highscores(void)
{
    unsigned char i;
    unsigned short address = 4;

    for (i = 0; i < HIGHSCORE_TABLE_SIZE; i++)
    {
        read_string(address, highscores[i].name, PLAYER_NAME_LENGTH + 1);
        address += strlen(highscores[i].name) + 1;
        highscores[i].score = read_int(address);
        address += 4;
    }
}

void reset_highscores(void)
{
    unsigned char i;
    unsigned short address = 4;

    for (i = 0; i < HIGHSCORE_TABLE_SIZE; i++)
    {
        write_string(address, "EMPTY", PLAYER_NAME_LENGTH + 1);
        address += strlen("EMPTY") + 1;
        write_int(address, 0);
        address += 4;
    }
}

Selection main_menu(void)
{
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

        if (btn4_clicked())
        {
            if (marked != SINGLE_PLAYER)
            {
                marked--;
            }
        }

        if (btn3_clicked())
        {
            if (marked != HIGHSCORE)
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

void single_player(void)
{
    Player player1;
    Player bot = {.name = "BOT"};
    Ball ball;
    Difficulty difficulty = difficulty_menu();
    input_name(player1.name, "ENTER YOUR NAME");
    reset_positions(&player1, &bot, &ball);

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

        if (btn4_down())
        {
            move_player_up(&player1);
        }
        if (btn3_down())
        {
            move_player_down(&player1);
        }

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
    input_name(player1.name, "ENTER NAME FOR PLAYER 1");
    input_name(player2.name, "ENTER NAME FOR PLAYER 2");

    reset_positions(&player1, &player2, &ball);

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

        if (btn4_down())
        {
            move_player_up(&player1);
        }
        if (btn3_down())
        {
            move_player_down(&player1);
        }

        if (btn2_down())
        {
            move_player_up(&player2);
        }
        if (btn1_down())
        {
            move_player_down(&player2);
        }

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
                y += 8;
            }
        }

        if (btn3_clicked())
        {
            if (y != -88)
            {
                y -= 8;
            }
        }

        if (btn2_clicked())
        {
            return;
        }
    }
}

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

    int i;
    for (i = 0; i < PLAYER_NAME_LENGTH; i++)
    {
        buffer[i] = 0x2D;
    }

    buffer[PLAYER_NAME_LENGTH] = '\0';

    while (1)
    {
        delay_ms(REFRESH_DELAY);
        clear_display();
        draw_string(pageTitle, 1, DISPLAY_WIDTH / 2 - get_string_width(pageTitle, 1) / 2, 1);
        draw_dotted_horizontal_line(0, 8, DISPLAY_WIDTH - 1, 2, 1);
        draw_char(marked, 10, 20);
        draw_rectangle(4, 15, 16, 15);
        draw_string(buffer, 2, 30, 20);
        refresh_display();

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
            buffer[digits++] = marked;
        }

        if (btn1_clicked() || digits == 10)
        {
            buffer[digits] = '\0';
            return;
        }
    }
}

void move_player_up(Player *player)
{
    if (player->y != 0)
    {
        player->y--;
    }
}
void move_player_down(Player *player)
{
    if (player->y + PLAYER_HEIGHT != DISPLAY_HEIGHT)
    {
        player->y++;
    }
}

void ai_control(Player *player, Ball *ball, Difficulty difficulty)
{
    static int actionPoints = 0;
    static int rounds = 0;

    if (rounds == 0)
    {
        actionPoints = random(0, 1000 + (1500 << difficulty));
        rounds = 50;
    }

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
    else if (actionPoints >= 500 && actionPoints <= 1000 && ball->velocityX == 1)
    {
        // Stall.
        rounds--;
    }
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
    player1->x = 2;
    player1->y = DISPLAY_HEIGHT / 2 - PLAYER_HEIGHT / 2;
    player2->x = DISPLAY_WIDTH - PLAYER_WIDTH - 2;
    player2->y = DISPLAY_HEIGHT / 2 - PLAYER_HEIGHT / 2;
    ball->x = DISPLAY_WIDTH / 2 - BALL_SIZE / 2;
    ball->y = DISPLAY_HEIGHT / 2 - BALL_SIZE / 2;
}

void draw_game(const Player *player1, const Player *player2, const Ball *ball, const int *gameTime)
{
    draw_filled_rectangle(player1->x, player1->y, PLAYER_WIDTH, PLAYER_HEIGHT);
    draw_filled_rectangle(player2->x, player2->y, PLAYER_WIDTH, PLAYER_HEIGHT);
    draw_filled_rectangle(ball->x, ball->y, BALL_SIZE, BALL_SIZE);
    draw_dotted_vertical_line(DISPLAY_WIDTH / 2, 0, 26, 4, 3);
    draw_int(player1->score, 1, DISPLAY_WIDTH / 2 - get_int_width(player1->score, 1) - 16, 0);
    draw_int(player2->score, 1, DISPLAY_WIDTH / 2 + get_int_width(player2->score, 1) / 2 + 15, 0);
    unsigned int minutes = (*gameTime) / 1000 / 60;
    unsigned int seconds = (*gameTime) / 1000 % 60;
    draw_int(minutes / 10, 0, 51, 27);
    draw_int(minutes % 10, 0, 57, 27);
    draw_char(':', 64, 27);
    draw_int(seconds / 10, 0, 68, 27);
    draw_int(seconds % 10, 0, 74, 27);
}

void update_game(Player *player1, Player *player2, Ball *ball, int *gameTime)
{
    if (reset)
    {
        reset_positions(player1, player2, ball);
        clear_display();
        draw_game(player1, player2, ball, gameTime);
        refresh_display();

        int ballRandomStart = random(0, 4);

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

        while (!(btn1_clicked() || btn2_clicked() || btn3_clicked() || btn4_clicked()))
            ;

        reset = 0;
    }
    else
    {
        *gameTime -= REFRESH_DELAY;

        if (ball->y == 0 || ball->y + BALL_SIZE == DISPLAY_HEIGHT)
        {
            ball->velocityY *= -1;
        }

        if (ball->x == player1->x + PLAYER_WIDTH && ball->y + BALL_SIZE >= player1->y && ball->y <= player1->y + PLAYER_HEIGHT)
        {
            ball->velocityX *= -1;
        }

        if (ball->x + BALL_SIZE == player2->x && ball->y + BALL_SIZE >= player2->y && ball->y <= player2->y + PLAYER_HEIGHT)
        {
            ball->velocityX *= -1;
        }

        if (ball->x + BALL_SIZE == DISPLAY_WIDTH)
        {
            player1->score++;
            reset = 1;
        }
        else if (ball->x == 0)
        {
            player2->score++;
            reset = 1;
        }
        else
        {
            ball->x += ball->velocityX;
            ball->y += ball->velocityY;
        }
    }
}

void update_highscore(Player *players, unsigned char numberOfPlayers)
{
    int playerIndex;
    for (playerIndex = 0; playerIndex < numberOfPlayers; playerIndex++)
    {
        Player newHighscores[9];

        int recordIndex;
        for (recordIndex = 0; recordIndex < HIGHSCORE_TABLE_SIZE; recordIndex++)
        {
            if (players[playerIndex].score >= highscores[recordIndex].score)
            {
                int i;
                newHighscores[recordIndex] = players[playerIndex];
                for (i = recordIndex; i < HIGHSCORE_TABLE_SIZE - 1; i++)
                {
                    newHighscores[i + 1] = highscores[i];
                }
                break;
            }

            newHighscores[recordIndex] = highscores[recordIndex];
        }

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
        write_string(address, highscores[i].name, PLAYER_NAME_LENGTH + 1);
        address += strlen(highscores[i].name) + 1;
        write_int(address, highscores[i].score);
        address += 4;
    }
}