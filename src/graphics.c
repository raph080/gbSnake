#include "graphics.h"

#include <rand.h>
#include <resources/sets.h>
#include <resources/snake.h>
#include <resources/snake_sleep.h>

/**
 * @defgroup SCREEN_COLORS Screen Colors
 *
 * @brief Represent the 4 possible colors of the gameboy screen.
 * @{
 */
#define WHITE      0
#define LIGHT_GRAY 1
#define DARK_GRAY  2
#define BLACK      3
/** @} */

/**
 * @defgroup SET_ORIGINS Set origins
 *
 * @brief the origin indices are the first tile index of the wanted data
 * from sets.bkg.png
 * @{
 */
#define MENU_BKG_ORIGIN        120
#define PRESS_START_TXT_ORIGIN 400
#define BOARD_BKG_ORIGIN       480
#define GAME_OVER_WIN_ORIGIN   840
/** @} */

/** the two snake animation (awake and sleeping) are composed of 10 frames */
#define SNAKE_FRAME_COUNT 10

/** when checking sets.bkg.png, the 0 tile is the 64 different tile from
// the begining of the image */
#define DIGIT_0_ORIGIN 64

/*************************************************
**              private functions               **
*************************************************/

/**
 * @brief Set the background palette to (c0, c1, c2, c3) with
 * cx in [WHITE, LIGHT_GRAY, DARK_GRAY, BLACK]
 *
 * @param c0 the first color of the palette
 * @param c1 the second color of the palette
 * @param c2 the third color of the palette
 * @param c3 the fourth color of the palette
 */
void SetBkgPalette(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3)
{
    BGP_REG = c0 | c1 << 2 | c2 << 4 | c3 << 6;
}

/**
 * @brief Set the sprite palette to (c0, c1, c2, c3) with
 * cx in [WHITE, LIGHT_GRAY, DARK_GRAY, BLACK]
 *
 * @param c0 the first color of the palette
 * @param c1 the second color of the palette
 * @param c2 the third color of the palette
 * @param c3 the fourth color of the palette
 */
void SetSpritePalette(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3)
{
    OBP0_REG = c0 | c1 << 2 | c2 << 4 | c3 << 6;
}

/**
 * @brief Set the Brightness of the screen (0 to BKG_BRIGHTNESS_MAX)
 *
 * @param brightness
 */
void SetBrightness(uint8_t brightness)
{
    if (brightness == 0) {
        SetBkgPalette(BLACK, BLACK, BLACK, BLACK);
        SetSpritePalette(BLACK, BLACK, BLACK, BLACK);
    }
    else if (brightness == 1) {
        SetBkgPalette(DARK_GRAY, BLACK, BLACK, BLACK);
        SetSpritePalette(BLACK, BLACK, BLACK, DARK_GRAY);
    }
    else if (brightness == 2) {
        SetBkgPalette(LIGHT_GRAY, DARK_GRAY, BLACK, BLACK);
        SetSpritePalette(BLACK, DARK_GRAY, BLACK, LIGHT_GRAY);
    }
    else {
        SetBkgPalette(WHITE, LIGHT_GRAY, DARK_GRAY, BLACK);
        SetSpritePalette(BLACK, LIGHT_GRAY, DARK_GRAY, WHITE);
    }
}

/**
 * @brief Display a given frame of the AWAKE_SNAKE_ID snake
 *
 * @param frame the frame to display (0 to SNAKE_FRAME_COUNT)
 */
void DisplaySnakeSprite(uint8_t frame)
{
    set_sprite_tile(0, frame * 2);
    set_sprite_tile(1, frame * 2 + 1);
    set_sprite_tile(2, frame * 2 + 20);
    set_sprite_tile(3, frame * 2 + 21);
}

/**
 * @brief Display a given frame of the SLEEPING_SNAKE_ID snake
 *
 * @param frame the frame to display (0 to SNAKE_FRAME_COUNT)
 */
void DisplaySnakeSleepSprite(uint8_t frame)
{
    set_sprite_tile(0, snake_TILE_COUNT + frame * 2);
    set_sprite_tile(1, snake_TILE_COUNT + frame * 2 + 1);
    set_sprite_tile(2, snake_TILE_COUNT + frame * 2 + 20);
    set_sprite_tile(3, snake_TILE_COUNT + frame * 2 + 21);
}

/**
 * @brief Set the default palette of the background and sprites.
 *
 */
void SetDefaultPalette()
{
    SetBkgPalette(WHITE, LIGHT_GRAY, DARK_GRAY, BLACK);
    SetSpritePalette(BLACK, LIGHT_GRAY, DARK_GRAY, WHITE);
}

/**
 * @brief Set a random palette for the background and sprites.
 *
 */
void SetRandomPalette()
{
    uint8_t seed = rand() % 4;

    if (seed == 0) {
        SetBkgPalette(BLACK, DARK_GRAY, BLACK, DARK_GRAY);
        SetSpritePalette(BLACK, DARK_GRAY, BLACK, DARK_GRAY);
    }
    else if (seed == 1) {
        SetBkgPalette(LIGHT_GRAY, WHITE, BLACK, DARK_GRAY);
        SetSpritePalette(BLACK, WHITE, DARK_GRAY, WHITE);
    }
    else if (seed == 2) {
        SetBkgPalette(BLACK, DARK_GRAY, BLACK, BLACK);
        SetSpritePalette(BLACK, DARK_GRAY, BLACK, LIGHT_GRAY);
    }
    else {
        SetBkgPalette(WHITE, LIGHT_GRAY, WHITE, LIGHT_GRAY);
        SetSpritePalette(BLACK, LIGHT_GRAY, WHITE, WHITE);
    }
}

/*************************************************
**               public functions               **
*************************************************/

void ShowMenuBkg()
{
    set_bkg_tiles(0, 0, 20, 18, sets_map + MENU_BKG_ORIGIN);
    SHOW_BKG;
}

void ShowBoardBkg()
{
    set_bkg_tiles(0, 0, 20, 18, sets_map + BOARD_BKG_ORIGIN);
    SHOW_BKG;
}

void ShowWin()
{
    set_win_tiles(0, 0, 20, 18, sets_map + GAME_OVER_WIN_ORIGIN);
    SHOW_WIN;
}

void HideWin()
{
    HIDE_WIN;
}

void ShowStartText()
{
    // set back the map of the "press start" row
    set_bkg_tiles(0, 14, 20, 1, sets_map + PRESS_START_TXT_ORIGIN);
}

void HideStartText()
{
    // replace every characters of "press start" to black (id=0)
    const unsigned char map[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    set_bkg_tiles(4, 14, 12, 1, map);
}

void CollapseWin()
{
    move_win(7, 136);
}

BlinkingStartTextState CreateBlinkingStartTextState(uint8_t timer,
                                                    BOOLEAN isVisibleAtStart,
                                                    int8_t startDelay)
{
    BlinkingStartTextState state = {timer, isVisibleAtStart, startDelay, 0};
    return state;
}

SnakeAnimState CreateSnakeAnimState(uint8_t timer, BOOLEAN isLoop,
                                    uint8_t startDelay,
                                    uint8_t waitBetweenLoop, uint8_t snakeId)
{
    SnakeAnimState state = {timer,   isLoop, startDelay, waitBetweenLoop,
                            snakeId, 0,      0};
    return state;
}

ExpandWinState CreateExpandWinState(uint8_t timer, uint8_t speed)
{
    ExpandWinState state = {timer, speed, 0};
    return state;
}
FadeState CreateFadeState(uint8_t timer)
{
    FadeState state = {timer, 0, 0};
    return state;
}

FlashState CreateFlashState(uint8_t timer, uint8_t duration)
{
    FlashState state = {timer, duration, 0};
    return state;
}

BOOLEAN UpdateExpandWin(ExpandWinState* state)
{
    if (WY_REG <= 1) return FALSE;

    state->curTime++;

    if (state->curTime % state->timer == 0) scroll_win(0, -state->speed);
    return TRUE;
}

BOOLEAN UpdateBlinkingStartTextState(BlinkingStartTextState* state)
{
    if (state->curTime == 0) {
        if (state->isVisibleAtStart)
            ShowStartText();
        else
            HideStartText();
    }
    else if (state->curTime >= state->startDelay &&
             (state->curTime - state->startDelay) % state->timer == 0) {
        if (state->isVisibleAtStart)
            ShowStartText();
        else
            HideStartText();

        state->isVisibleAtStart = (state->isVisibleAtStart) ? FALSE : TRUE;
    }

    state->curTime++;

    return TRUE;
}

BOOLEAN UpdateSnakeAnimState(SnakeAnimState* state)
{
    // finish if anim not looping and all frames are shown
    if (!state->isLoop && state->curTime >= state->timer * (SNAKE_FRAME_COUNT +
                                                            state->startDelay))
        return FALSE;

    if (state->curTime % state->timer == 0) {
        // frame helps to deal with the "wait between loop"
        uint8_t frame = state->curFrame;

        // if frame before startDelay or after SNAKE_FRAME_COUNT, frame -> 0
        if (frame < state->startDelay)
            frame = 0;
        else
            frame -= state->startDelay;

        if (frame >= SNAKE_FRAME_COUNT) frame = 0;

        // display the corresponding snake
        if (state->snakeId == AWAKE_SNAKE_ID)
            DisplaySnakeSprite(frame);
        else
            DisplaySnakeSleepSprite(frame);

        state->curFrame++;

        if (state->curFrame >=
            state->startDelay + SNAKE_FRAME_COUNT + state->waitBetweenLoop)
            state->curFrame = 0;
    }

    state->curTime++;

    return TRUE;
}

BOOLEAN UpdateFadeIn(FadeState* state)
{
    if (state->curTime > state->timer * BKG_BRIGHTNESS_MAX) return FALSE;

    if (state->curTime % state->timer == 0) {
        SetBrightness(state->curBrightness);
        state->curBrightness++;
    }

    state->curTime++;

    return TRUE;
}

BOOLEAN UpdateFadeOut(FadeState* state)
{
    if (state->curTime > state->timer * BKG_BRIGHTNESS_MAX) return FALSE;

    if (state->curTime % state->timer == 0) {
        SetBrightness(BKG_BRIGHTNESS_MAX - state->curBrightness);
        state->curBrightness++;
    }

    state->curTime++;

    return TRUE;
}

BOOLEAN UpdateFlash(FlashState* state)
{
    if (state->curTime > state->duration) {
        SetDefaultPalette();
        return FALSE;
    }

    if (state->curTime % state->timer == 0) {
        SetRandomPalette();
    }

    state->curTime++;

    return TRUE;
}

void ShowSnakeSprite()
{
    SHOW_SPRITES;
}

void HideSnakeSprite()
{
    HIDE_SPRITES;
}

void MoveSnakeSprite(uint8_t x, uint8_t y)
{
    move_sprite(0, x, y);
    move_sprite(1, x + 8, y);
    move_sprite(2, x, y + 8);
    move_sprite(3, x + 8, y + 8);
}

void ScrollSnakeSprite(int8_t x, int8_t y)
{
    scroll_sprite(0, x, y);
    scroll_sprite(1, x, y);
    scroll_sprite(2, x, y);
    scroll_sprite(3, x, y);
}

BoardCell GetBoardCell(uint8_t x, uint8_t y)
{
    BoardCell cell = get_bkg_tile_xy(x, y);

    // WALL_CELL is the index of the first wall tile. But there are
    // multiple wall tiles. Every tile after WALL_CELL will be considered
    // as a wallcheck. Check the first row of sets.bkg.png for more info.
    if (cell > WALL_CELL) cell = WALL_CELL;

    return cell;
}

void SetBoardCell(uint8_t x, uint8_t y, BoardCell cell)
{
    set_bkg_tile_xy(x, y, cell);
}

void SetLegendScore(uint8_t score)
{
    set_win_tile_xy(9, 0, DIGIT_0_ORIGIN + (score % 10));
    score /= 10;
    set_win_tile_xy(8, 0, DIGIT_0_ORIGIN + (score % 10));
    score /= 10;
    set_win_tile_xy(7, 0, DIGIT_0_ORIGIN + (score % 10));
}

void SetLegendLevel(uint8_t level)
{
    set_win_tile_xy(17, 0, DIGIT_0_ORIGIN + (level / 10));
    set_win_tile_xy(18, 0, DIGIT_0_ORIGIN + (level % 10));
}

void InitGraphics()
{
    set_bkg_data(0, sets_TILE_COUNT, sets_tiles);

    set_sprite_data(0, snake_TILE_COUNT, snake_tiles);
    set_sprite_data(snake_TILE_COUNT, snake_sleep_TILE_COUNT,
                    snake_sleep_tiles);

    SHOW_SPRITES;

    SetDefaultPalette();
    initrand(DIV_REG);
}
