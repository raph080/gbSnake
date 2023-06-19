#include "board.h"

#include <rand.h>
#include <stdlib.h>

#include "graphics.h"
#include "sound.h"
#include "utils.h"

/*************************************************
**                 structures                   **
*************************************************/

/** @struct SnakeNode
 *  Represent a snake node with its position and a pointer to its direct child
 *  (the head being the root parent).
 *
 *  @var SnakeNode::x
 *    The x position of the node in the board.
 *  @var SnakeNode::y
 *    The y position of the node in the board.
 *  @var SnakeNode::next
 *    A pointer to the direct child of the node
 */
struct SnakeNode {
    uint8_t x, y;
    struct SnakeNode* next;
};

typedef struct SnakeNode SnakeNode;

/** @struct Snake
 *  Represent a snake with pointers to its head and tail
 *
 *  @var Snake::head
 *    A pointer to the head node of the snake (root parent node).
 *  @var Snake::tail
 *    A pointer to the tail node of the snake.
 */
typedef struct {
    SnakeNode* head;
    SnakeNode* tail;
} Snake;

/*************************************************
**             private functions                **
*************************************************/

/**
 * @brief Initialize the given snake (already allocated).
 *
 * @param snake a pointer to a valid Snake.
 */
void InitSnake(Snake* snake)
{
    snake->head = NULL;
    snake->tail = NULL;
}

/**
 * @brief Prepend a new root node to the snake and set x and y positions to
 * this new node.
 *
 * @param snake a pointer to a valid Snake
 * @param x the x position of the new node
 * @param y the y position of the new node
 */
void PrependSnakeNode(Snake* snake, uint8_t x, uint8_t y)
{
    SnakeNode* newNode = (SnakeNode*)malloc(sizeof(SnakeNode));
    newNode->x = x;
    newNode->y = y;
    newNode->next = snake->head;
    snake->head = newNode;

    // null tail means the snake has previously no node
    if (snake->tail == NULL) snake->tail = newNode;
}

/**
 * @brief Remove the tail node, prepend it as head node of the snake and
 * set x and y as new positions. This is a small optimisation to avoid
 * freeing the tail and allocating a new head.
 *
 * @param snake a pointer to a valid Snake
 * @param x the x position of the new head node
 * @param y the y position of the new head node
 */
void PrependLastSnakeNode(Snake* snake, uint8_t x, uint8_t y)
{
    SnakeNode* prevNode = NULL;
    SnakeNode* curNode = snake->head;
    while (curNode->next != NULL) {
        prevNode = curNode;
        curNode = curNode->next;
    }
    // the last prev node is now the last node
    if (prevNode) {
        prevNode->next = NULL;
        snake->tail = prevNode;
        curNode->next = snake->head;
    }

    curNode->x = x;
    curNode->y = y;
    snake->head = curNode;
}

/**
 * @brief Add a random loot to the play board.
 *
 */
void AddRandomLootToBoard()
{
    while (TRUE) {
        uint8_t x = rand() % MAX_TILE_WIDTH;
        uint8_t y = rand() % MAX_TILE_HEIGHT - 1;

        BoardCell cell = GetBoardCell(x, y);
        if (cell == EMPTY_CELL) {
            SetBoardCell(x, y, LOOT_CELL);
            break;
        }
    }
}

/*************************************************
**               public functions               **
*************************************************/

void RunBoard()
{
    /****  prepare  ****/

    ShowBoardBkg();
    CollapseWin();
    ShowWin();
    StopSound();
    HideSnakeSprite();

    /****  fade in  ****/

    BOOLEAN isFadeIn = TRUE;

    FadeState fadeState;
    fadeState = CreateFadeState(6);

    while (isFadeIn) {
        isFadeIn = UpdateFadeIn(&fadeState);
        Delay(1);
    }

    /****  play sound  ****/

    PlayBoardSound(TRUE);

    /****  init snake  ****/

    Snake snake;
    InitSnake(&snake);

    uint8_t snakeDir = J_RIGHT;
    uint8_t snakeTimer = 10;
    uint16_t lootTimer = ((uint16_t)rand()) << 0;
    uint8_t snakeSize = 1;
    uint8_t level = 1;

    SetLegendScore(snakeSize);
    SetLegendLevel(level);

    // find the snake node from the board
    for (uint8_t x = 0; x < MAX_TILE_WIDTH; x++) {
        for (uint8_t y = 0; y < MAX_TILE_HEIGHT - 1; y++) {
            BoardCell cell = GetBoardCell(x, y);
            if (cell == SNAKE_CELL) PrependSnakeNode(&snake, x, y);
        }
    }

    /****  game loop  ****/

    while (TRUE) {
        // update the snake direction given the joypad input
        // by rule, the cannot switch to opposite direction
        if (joypad() & J_UP && snakeDir != J_DOWN) snakeDir = J_UP;
        if (joypad() & J_DOWN && snakeDir != J_UP) snakeDir = J_DOWN;
        if (joypad() & J_RIGHT && snakeDir != J_LEFT) snakeDir = J_RIGHT;
        if (joypad() & J_LEFT && snakeDir != J_RIGHT) snakeDir = J_LEFT;

        snakeTimer--;
        if (snakeTimer == 0) {
            // equation to lower the timer as the level increase
            snakeTimer = 15 - (level * 2);

            SnakeNode newHead;
            newHead = *(snake.head);

            // update the snake head position
            switch (snakeDir) {
                case J_UP: newHead.y--; break;
                case J_DOWN: newHead.y++; break;
                case J_RIGHT: newHead.x++; break;
                case J_LEFT: newHead.x--; break;
                default: break;
            }

            // check game over
            BoardCell cell = GetBoardCell(newHead.x, newHead.y);

            if (cell == SNAKE_CELL || cell == WALL_CELL)
                return;

            else if (cell == LOOT_CELL) {
                PrependSnakeNode(&snake, newHead.x, newHead.y);

                snakeSize++;
                SetLegendScore(snakeSize);

                // each the the snake grow by 5, the level up
                if (snakeSize % 5 == 0) {
                    level++;
                    SetLegendLevel(level);
                }
            }
            else {
                // erase the last snake position
                SetBoardCell(snake.tail->x, snake.tail->y, EMPTY_CELL);

                PrependLastSnakeNode(&snake, newHead.x, newHead.y);
            }

            // print the snake
            SetBoardCell(snake.head->x, snake.head->y, SNAKE_CELL);
        }

        lootTimer--;
        if (lootTimer == 0) {
            lootTimer = ((uint16_t)rand()) << 0;

            AddRandomLootToBoard();
        }

        Delay(1);
        UpdateSound();
    }
}
