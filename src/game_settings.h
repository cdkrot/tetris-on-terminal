#ifndef __GAME_SETTINGS_H

#define __GAME_SETTINGS_H

#include "stdstyle.h"

struct game_settings
{
	uint32_t no_spawn_ticks;
	uint32_t freeze_ticks;
	uint32_t ticks_per_fall;
	bool rainbow_mode;
};

#endif
