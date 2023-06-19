/**
 * @file utils.h
 * @author Raphael Jouretz (rjouretz.com)
 * @brief Implement utility functions
 * @version 0.1
 * @date 2023-06-18
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <stdint.h>

#ifndef UTILS_H
#define UTILS_H

#define MAX_TILE_WIDTH  20
#define MAX_TILE_HEIGHT 18

/**
 * @brief Blocking wait
 *
 * @param nbCycles the number of vsync to wait
 */
void Delay(uint16_t nbCycles);

#endif