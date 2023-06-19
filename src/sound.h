/**
 * @file sound.h
 * @author Raphael Jouretz (rjouretz.com)
 * @brief The lib to interact with the audio
 * @version 0.1
 * @date 2023-06-18
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <gb/gb.h>
#include <stdint.h>
#include <types.h>

#ifndef SOUND_H
#define SOUND_H

/**
 * @brief Init the states of the Sound Player
 *
 */
void InitSoundPlayer();

void PlayMenuSound(BOOLEAN loop);

void PlayBoardSound(BOOLEAN loop);

void PlayGameOverSound(BOOLEAN loop);

/**
 * @brief Update the sound. Must be called every vsync
 *
 */
void UpdateSound();

/**
 * @brief Set the Volume of the Sound Player. 0x77 is max and 0x00 is no sound
 *
 * @param volume the volume to set
 */
void SetVolume(uint8_t volume);

/**
 * @brief Stop the audio
 *
 */
void StopSound();

#endif