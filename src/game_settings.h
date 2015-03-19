#ifndef __GAME_SETTINGS_H

#define __GAME_SETTINGS_H

#include "stdstyle.h"

struct game_settings
{
	uint32_t no_spawn_ticks;
	uint32_t freeze_ticks;
	uint32_t ticks_per_fall;
	
	// rainbow config.
	uint32_t ticks_per_rainbow;
	uint32_t ticks_per_rainbow_rand_fact;
	uint32_t ticks_per_invert;
	uint32_t ticks_per_invert_rand_fact;
};

#endif
