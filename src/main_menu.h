#ifndef __MAIN_MENU_H

#define __MAIN_MENU_H

#include "stdstyle.h"
#include "game_settings.h"
#include "Iscreen.h"

class main_menu: public iscreen
{
	public:
		main_menu(screen_manager* mgr): mgr(mgr){}
		virtual bool should_run();
		virtual void update_game();
		virtual void process_input();
		virtual void render();
	private:
		screen_manager* mgr;
		uint32_t sel = 0;
		bool is_running = true;
		
		// no_spawn_ticks - freeze_ticks - ticks_per_fall - rainbow_mode
		const game_settings normal  = {12, 25, 06, false};
		const game_settings hard    = {03,  25, 04, false};
		const game_settings rainbow = {01,  25, 04, true };

};

class menu_credits: public iscreen
{
	public:
		menu_credits(screen_manager* mgr): mgr(mgr){}
		virtual bool should_run();
		virtual void update_game();
		virtual void process_input();
		virtual void render();
	private:
		screen_manager* mgr;
};

#endif
