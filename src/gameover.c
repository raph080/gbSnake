#include "gameover.h"

#include "graphics.h"
#include "sound.h"
#include "utils.h"

void RunGameOver()
{
    /****  prepare  ****/

    StopSound();
    HideSnakeSprite();

    /****  play damage  ****/

    BOOLEAN isDamaged = TRUE;

    FlashState flashState;
    flashState = CreateFlashState(1, 24);

    while (isDamaged) {
        if (isDamaged) isDamaged = UpdateFlash(&flashState);
        Delay(1);
    }

    /****  wait  ****/

    Delay(40);

    /****  play sound  ****/

    PlayGameOverSound(FALSE);

    /****  set animation states  ****/

    BOOLEAN isWinExpanding = TRUE;
    BOOLEAN isSnakeAnimated = FALSE;
    BOOLEAN isFadeOut = FALSE;

    ExpandWinState expandWinState;
    SnakeAnimState snakeAnimState;
    FadeState fadeState;
    expandWinState = CreateExpandWinState(1, 1);
    snakeAnimState = CreateSnakeAnimState(8, FALSE, 8, 1, SLEEPING_SNAKE_ID);
    fadeState = CreateFadeState(12);

    // move the snake out of the screen so it will eventually scroll
    // to the correct position
    MoveSnakeSprite(80, 208);
    // update the first frame of the correct snake
    UpdateSnakeAnimState(&snakeAnimState);
    // show the first frame of the snake outside the screen
    ShowSnakeSprite();

    uint16_t gameOverTimer = 330;

    /****  game over loop  ****/

    while (TRUE) {
        gameOverTimer--;
        if (gameOverTimer == 0) isFadeOut = TRUE;

        if (isWinExpanding) {
            isWinExpanding = UpdateExpandWin(&expandWinState);
            if (isWinExpanding)
                ScrollSnakeSprite(0, -1);
            else
                isSnakeAnimated = TRUE;
        }
        if (isSnakeAnimated)
            isSnakeAnimated = UpdateSnakeAnimState(&snakeAnimState);

        if (isFadeOut) {
            isFadeOut = UpdateFadeOut(&fadeState);
            if (!isFadeOut) break;
        }

        UpdateSound();

        Delay(1);
    }

    Delay(50);

    HideWin();
}