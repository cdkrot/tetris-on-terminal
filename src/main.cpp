#include <iostream>
#include <time.h>
#include <signal.h>
#include "game.h"
#include "terminal.h"
#include "stdstyle.h"
#include <unistd.h>

void handle_signal(int)
{
	terminal_shutdown();
	exit(0);
}

int main()
{
	// according to POSIX CLOCKS_PER_SEC should be equal to specified value.
	// unfortunately not all systems, are posix.
	assert(CLOCKS_PER_SEC == 1000000);
	terminal_init();
	signal(SIGINT, handle_signal);
	srand(time(0));
	
	// thanks to gameprogrammingpatterns.com for the gameloop.
	const clock_t CLOCKS_PER_UPD = CLOCKS_PER_SEC / TICKS_PER_SEC;
	clock_t prev = clock();
	clock_t lag = 0;
	game the_game;
	while (the_game.should_run())
	{
		clock_t cur = clock();
		clock_t elapsed = cur - prev;
		prev = cur;
		lag += elapsed;
		
		if (lag >= clock_t(5 * CLOCKS_PER_SEC))
		{
			lag = clock_t(0); // warning: can't keep up.
			return 0; // TODO: revert.
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
			long nanosec = (CLOCKS_PER_UPD - lag) * 1000;
			timespec req = {0, nanosec};
			nanosleep(&req, NULL);
			lag = CLOCKS_PER_UPD;
		}
	}
	
	handle_signal(0); // just shutdown.
	
	return 0;
}
