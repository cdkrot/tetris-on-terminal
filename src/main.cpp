#include <iostream>
#include <time.h>
#include "game.h"
#include "terminal.h"
#include "stdstyle.h"

int main()
{
	// no_spawn_ticks - freeze_ticks - ticks_per_fall - rainbow_mode
	const game_settings normal  = {12, 25, 06, false};
	const game_settings hard    = {03,  25, 04, false};
	const game_settings rainbow = {01,  25, 04, true };
	
	terminal_init();
	srand(time(0));
	
	// thanks to gameprogrammingpatterns.com for the gameloop.
	const clock_t CLOCKS_PER_UPD = CLOCKS_PER_SEC / TICKS_PER_SEC;
	clock_t prev = clock();
	clock_t lag = 0;
	game the_game(normal);
	while (the_game.should_run())
	{
		clock_t cur = clock();
		clock_t elapsed = cur - prev;
		prev = cur;
		lag += elapsed;
		
		if (lag >= clock_t(5 * CLOCKS_PER_SEC))
		{
			lag = clock_t(0); // warning: can't keep up.
		}
		if (lag >= CLOCKS_PER_UPD)
		{
			while (lag >= CLOCKS_PER_UPD)
			{
				lag -= CLOCKS_PER_UPD;
				// this is not a canonical game loop, but we need this accuracy here.
				the_game.process_input();
				the_game.update_game();
			}
			
			the_game.render();
		}
		else
		{
			// lag < CLOCKS_PER_UPD
			
			// it is expected to CLOCKS_PER_SEC to be equal 10^6, but it is not required.
			long nanosec = (CLOCKS_PER_UPD - lag) * (1000000000 / CLOCKS_PER_SEC);
			timespec req = {0, nanosec};
			nanosleep(&req, NULL);
			lag = CLOCKS_PER_UPD;
		}
	}
	
	terminal_shutdown();
	
	return 0;
}
