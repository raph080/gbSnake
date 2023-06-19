#include "utils.h"

#include <gb/gb.h>

void Delay(uint16_t nbCycles)
{
    for (uint16_t i = 0; i < nbCycles; i++) wait_vbl_done();
}