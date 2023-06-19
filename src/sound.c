#include "sound.h"

#include <gbt_player.h>

// created globally by gbt_player at compile time
extern const unsigned char* menu_music_Data[];
extern const unsigned char* board_music_Data[];
extern const unsigned char* game_over_music_Data[];

/*************************************************
**              private functions               **
*************************************************/

/**
 * @brief Play the given track
 *
 * @param track     the track to play
 * @param loop      true if play the track in loop. false otherwise
 */
void PlaySound(const unsigned char** track, BOOLEAN loop)
{
    gbt_play(track, 2, 7);
    gbt_loop(loop);
}

/*************************************************
**               public functions               **
*************************************************/

void InitSoundPlayer()
{
    disable_interrupts();

    set_interrupts(VBL_IFLAG);
    enable_interrupts();
}

void PlayMenuSound(BOOLEAN loop)
{
    PlaySound(menu_music_Data, loop);
}

void PlayBoardSound(BOOLEAN loop)
{
    PlaySound(board_music_Data, loop);
}

void PlayGameOverSound(BOOLEAN loop)
{
    PlaySound(game_over_music_Data, loop);
}

void UpdateSound()
{
    gbt_update();
}

void SetVolume(uint8_t volume)
{
    NR50_REG = volume;
}

void StopSound()
{
    gbt_stop();
}