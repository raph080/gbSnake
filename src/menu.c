#include "menu.h"

#include <types.h>

#include "graphics.h"
#include "sound.h"
#include "utils.h"

void RunMenu()
{
    /****  prepare  ****/

    ShowMenuBkg();
    MoveSnakeSprite(84, 34);

    /****  play audio  ****/

    PlayMenuSound(TRUE);

    /****  set animation states  ****/

    BOOLEAN isFadeIn = TRUE;
    BOOLEAN isFadeOut = FALSE;
    BOOLEAN isSnakeAnimated = TRUE;
    BOOLEAN isStartPressed = FALSE;

    FadeState fadeState;
    SnakeAnimState snakeAnimState;
    BlinkingStartTextState blinkingStartTextState;

    fadeState = CreateFadeState(6);
    snakeAnimState = CreateSnakeAnimState(8, TRUE, 12, 12, AWAKE_SNAKE_ID);
    blinkingStartTextState = CreateBlinkingStartTextState(24, FALSE, 24);

    UpdateFadeIn(&fadeState);

    /****  menu loop  ****/

    while (TRUE) {
        // handle the joypad
        if (joypad() & J_START) {
            fadeState = CreateFadeState(6);
            isFadeOut = TRUE;
            isStartPressed = TRUE;
        }
        // update the animations
        if (isFadeIn) isFadeIn = UpdateFadeIn(&fadeState);

        if (isSnakeAnimated)
            isSnakeAnimated = UpdateSnakeAnimState(&snakeAnimState);

        if (isFadeOut) {
            isFadeOut = UpdateFadeOut(&fadeState);
            // fade out finished -> quit menu
            if (!isFadeOut && isStartPressed) break;
        }

        UpdateBlinkingStartTextState(&blinkingStartTextState);

        // update the sound
        UpdateSound();

        // delay to limit framerate
        Delay(1);
    }
}