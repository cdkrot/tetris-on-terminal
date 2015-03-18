#include <memory>
#include <time.h>
#include "terminal.h"
#include "Iscreen.h"
#include "main_menu.h"
#include "stdstyle.h"

int main()
{
	terminal_init();
	srand(time(0));
	
	// thanks to gameprogrammingpatterns.com for the gameloop.
	const clock_t CLOCKS_PER_UPD = CLOCKS_PER_SEC / TICKS_PER_SEC;
	clock_t prev = clock();
	clock_t lag = 0;
	screen_manager scr_mgr;
	scr_mgr.push_screen(std::shared_ptr<iscreen>(new main_menu(&scr_mgr)));
	while (scr_mgr.should_run())
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
				scr_mgr.process_input();
				scr_mgr.update_game();
			}
			
			scr_mgr.render();
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
	printf("# Bye #\n");
	
	return 0;
}
