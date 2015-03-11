#include <iostream>
#include <time.h>
#include <signal.h>
#include "game.h"
#include "terminal.h"
#include "stdstyle.h"

void handle_signal(int)
{
	terminal_shutdown();
	exit(0);
}

int main()
{
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
		
		the_game.process_input();
		
		
		while (lag >= CLOCKS_PER_UPD)
		{
			lag -= CLOCKS_PER_UPD;
			the_game.update_game();
		}
		
		the_game.render();
	}
	
	handle_signal(0); // just shutdown.
	
	return 0;
}
