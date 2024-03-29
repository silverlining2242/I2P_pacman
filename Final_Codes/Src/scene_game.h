// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

#ifndef SCENE_GAME_H
#define SCENE_GAME_H
#include "game.h"

Scene scene_main_create(void);

int32_t set_power_up_timer_tick(int mode);            // change from get_power_up_timer_tick();
void set_cool_timer_pman2(Pacman *pman, int mode); // TODO-MC
int32_t get_power_up_duration();
void set_reverse_timer(Pacman *pman, int mode);

extern void stop_PACMAN_POWERUPSOUND();

#endif
