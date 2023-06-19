/**
 * @file main.c
 * @author Raphael Jouretz
 * @brief Main file containing entry function of the snake game
 * @version 0.1
 * @date 2023-06-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "board.h"
#include "gameover.h"
#include "graphics.h"
#include "menu.h"
#include "sound.h"
#include "utils.h"

/**
 * @brief Initialize the gameboy startup states
 *
 */
void InitGameBoy()
{
    InitSoundPlayer();
    InitGraphics();
}

/**
 * @brief Main function of the game
 *
 */
void main()
{
    InitGameBoy();

    Delay(10);

    while (TRUE) {
        RunMenu();
        RunBoard();
        RunGameOver();
    }
}