/**
 * @file graphics.h
 * @author Raphael Jouretz (rjouretz.com)
 * @brief The lib to interact with the gameboy screen
 * @version 0.1
 * @date 2023-06-18
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <gb/gb.h>
#include <stdint.h>
#include <types.h>

#ifndef GRAPHICS_H
#define GRAPHICS_H

#define BKG_BRIGHTNESS_MAX 3

#define AWAKE_SNAKE_ID     0
#define SLEEPING_SNAKE_ID  1

/** \enum BoardCell
 * \brief Represent a cell within the game board.
 *
 *  See the first tile row of sets.bkg.png to have more info about the order.
 */
typedef enum {
    EMPTY_CELL = 1,
    SNAKE_CELL,
    LOOT_CELL,
    WALL_CELL
} BoardCell;

/** @struct BlinkingStartTextState
 *  Represent a state of the blinking "press START" text from the menu screen.
 *
 *  @var BlinkingStartTextState::timer
 *    The timer that loops and that triggers the blinking every time it
 *    reaches 0
 *  @var BlinkingStartTextState::isVisibleAtStart
 *    True if "press START" is visible before any blinking
 *  @var BlinkingStartTextState::startDelay
 *    The delay before the first blinking
 *  @var BlinkingStartTextState::curTime
 *    The number of time this state has been updated from start
 */
typedef struct {
    uint8_t timer;
    BOOLEAN isVisibleAtStart;
    uint8_t startDelay;
    uint8_t curTime;
} BlinkingStartTextState;

/** @struct ExpandWinState
 *  Represent a state of the expansion of the window with the score and
 *  Game Over text.
 *
 *  @var ExpandWinState::timer
 *    The timer that loops and that triggers the expand every time it
 *    reaches 0
 *  @var ExpandWinState::speed
 *    The number of pixel the window expands for every update.
 *  @var ExpandWinState::curTime
 *    The number of time this state has been updated from start
 */
typedef struct {
    uint8_t timer;
    uint8_t speed;
    uint8_t curTime;
} ExpandWinState;

/** @struct FadeState
 *  Represent a state of a screen fading (in or out).
 *
 *  @var FadeState::timer
 *    The timer that loops and that triggers the fade every time it
 *    reaches 0
 *  @var FadeState::curBrightness
 *    The current brightness of the screen (0 to BKG_BRIGHTNESS_MAX)
 *  @var FadeState::curTime
 *    The number of time this state has been updated from start
 */
typedef struct {
    uint8_t timer;
    uint8_t curBrightness;
    uint8_t curTime;
} FadeState;

/** @struct FlashState
 *  Represent a state of a screen flashing.
 *
 *  @var FlashState::timer
 *    The timer that loops and that triggers a flash every time it
 *    reaches 0
 *  @var FlashState::duration
 *    The duration of the flash
 *  @var FlashState::curTime
 *    The number of time this state has been updated from start
 */
typedef struct {
    uint8_t timer;
    uint8_t duration;
    uint8_t curTime;
} FlashState;

/** @struct SnakeAnimState
 *  Represent a state of an animation of a snalke (AWAKE or SLEEPING)
 *
 *  @var SnakeAnimState::timer
 *    The timer that loops and that triggers a new frame every time it
 *    reaches 0
 *  @var SnakeAnimState::isLoop
 *    True of the animation loops
 *  @var BlinkingStartTextState::startDelay
 *    The delay before the first frame update
 *  @var SnakeAnimState::waitBetweenLoop
 *    The delay between two different loops (if isLoop = true)
 *  @var SnakeAnimState::snakeId
 *    The id of the snake to animate (AWAKE_SNAKE_ID or SLEEPING_SNAKE_ID)
 *  @var SnakeAnimState::curTime
 *    The number of time this state has been updated from start
 *  @var SnakeAnimState::curFrame
 *    The number of frame already displayed (including wait and delay frames)
 */
typedef struct {
    uint8_t timer;
    BOOLEAN isLoop;
    uint8_t startDelay;
    uint8_t waitBetweenLoop;
    uint8_t snakeId;
    uint8_t curTime;
    uint8_t curFrame;
} SnakeAnimState;

/**
 * @brief Initialize the graphics. Must be called before any other graphics
 * functions.
 *
 */
void InitGraphics();

/**
 * @brief Show the menu to the screen as background
 *
 */
void ShowMenuBkg();

/**
 * @brief Show the board to the screen as background
 *
 */
void ShowBoardBkg();

/**
 * @brief Show the window (layer with the scores and game over text)
 *
 */
void ShowWin();

/**
 * @brief Hide the window (layer with the scores and game over text)
 *
 */
void HideWin();

/**
 * @brief Collapse the window in order to only see the scores at the last
 * row of the screen (no game over text).
 *
 */
void CollapseWin();

/**
 * @brief Show the "press START" text at the menu screen
 *
 */
void ShowStartText();

/**
 * @brief Hide the "press START" text at the menu screen
 *
 */
void HideStartText();

/**
 * @brief Show the snake sprite (according to the snake id that was
 * previously set).
 *
 */
void ShowSnakeSprite();

/**
 * @brief Hide the snake sprite
 *
 */
void HideSnakeSprite();

/**
 * @brief Scroll the snake sprite to relative x and y positions
 *
 * @param x the relative x position to scroll
 * @param y the relative y position to scroll
 */
void ScrollSnakeSprite(int8_t x, int8_t y);

/**
 * @brief Move the snake sprite to the absolution x and y positions
 *
 * @param x the x position
 * @param y the y position
 */
void MoveSnakeSprite(uint8_t x, uint8_t y);

/**
 * @brief Get the board cell at the given (x,y) position
 *
 * @param x the x position
 * @param y the y position
 * @return the board cell at the (x,y) position
 */
BoardCell GetBoardCell(uint8_t x, uint8_t y);

/**
 * @brief Set the given board cell at the given (x,y) position of the board.
 *
 * @param x the x position
 * @param y the y position
 * @param cell the cell to set
 */
void SetBoardCell(uint8_t x, uint8_t y, BoardCell cell);

/**
 * @brief Set the value of the score within the legend
 *
 * @param score the score value to set
 */
void SetLegendScore(uint8_t score);

/**
 * @brief Set the value of the level within the legend
 *
 * @param level the level value to set
 */
void SetLegendLevel(uint8_t level);

/**
 * @brief Create a FadeState struct and initialize it with the given arguments.
 *
 * @param timer the timer to set within the new FadeState struct
 * @return the newly created FadeState struct
 */
FadeState CreateFadeState(uint8_t timer);

/**
 * @brief Create a FlashState struct and initialize it with the given
 * arguments.
 *
 * @param timer the timer to set
 * @param duration the duration to set
 * @return the newly created FlashState struct
 */
FlashState CreateFlashState(uint8_t timer, uint8_t duration);

/**
 * @brief Create an ExpandWinState struct and initialize it with the given
 * arguments.
 *
 * @param timer the timer to set
 * @param speed the speed to set
 * @return the newly created ExpandWinState struct
 */
ExpandWinState CreateExpandWinState(uint8_t timer, uint8_t speed);

/**
 * @brief Create a SnakeAnimState struct and initialize it with the given
 * arguments.
 *
 * @param timer the timer to set
 * @param isLoop true of the animation loops
 * @param startDelay the delay before the first frame update
 * @param waitBetweenLoop the delay between two different loops (if isLoop =
 * true)
 * @param snakeId the id of the snake to animate (AWAKE_SNAKE_ID or
 * SLEEPING_SNAKE_ID)
 * @return the newly created SnakeAnimState struct
 */
SnakeAnimState CreateSnakeAnimState(uint8_t timer, BOOLEAN isLoop,
                                    uint8_t startDelay,
                                    uint8_t waitBetweenLoop, uint8_t snakeId);

/**
 * @brief Create a BlinkingStartTextState struct and initialize it with the
 * given arguments.
 *
 * @param timer the timer to set
 * @param isVisibleAtStart true if "press START" is visible before any blinking
 * @param startDelay the delay before the first blinking
 * @return the newly created BlinkingStartTextState struct
 */
BlinkingStartTextState CreateBlinkingStartTextState(uint8_t timer,
                                                    BOOLEAN isVisibleAtStart,
                                                    int8_t startDelay);

/**
 * @brief Update the given FadeState struct and fade in the screen accordingly.
 *
 * @param state the structure storing the state of the fade in.
 * @return True if fade in was applied to the screen. False if the screen was
 * already at max brightness (BKG_BRIGHTNESS_MAX)
 */
BOOLEAN UpdateFadeIn(FadeState* state);

/**
 * @brief Update the given FadeState struct and fade out the screen
 * accordingly.
 *
 * @param state the structure storing the state of the fade out.
 * @return True if fade out was applied to the screen. False if the screen was
 * already at lower brightness (0)
 */
BOOLEAN UpdateFadeOut(FadeState* state);

/**
 * @brief Update the given FlashState struct and apply a flash accordingly.
 *
 * @param state the structure storing the state of the flash effect.
 * @return True if a flash effect was applied to the screen. False if the
 * effect is over.
 */
BOOLEAN UpdateFlash(FlashState* state);

/**
 * @brief Update the given ExpandWinState struct and expand the window
 * accordingly.
 *
 * @param state the structure storing the state of the window expansion.
 * @return True if the window was still expanding. False if the window was
 * already fully expanded.
 */
BOOLEAN UpdateExpandWin(ExpandWinState* state);

/**
 * @brief Update the given SnakeAnimState struct and update the frame
 * accordingly.
 *
 * @param state the structure storing the state of the snake animation.
 * @return True if the frame has been updated. False otherwise.
 */
BOOLEAN UpdateSnakeAnimState(SnakeAnimState* state);

/**
 * @brief Update the given BlinkingStartTextState struct and update the
 * "press Start" text visibility accordingly.
 *
 * @param state the structure storing the state of the blinking effect.
 * @return True if a blinking effect was applied to the screen. False if the
 * effect is over.
 */
BOOLEAN UpdateBlinkingStartTextState(BlinkingStartTextState* state);

#endif