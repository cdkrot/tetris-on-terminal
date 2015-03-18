#ifndef __GAME_H

#define __GAME_H

#include "stdstyle.h"
#include "char_data.h"
#include "figures.h"
#include <vector>
using std::vector;

class game;

class cur_figure_manager
{
	public:
		cur_figure_manager() = default;
		~cur_figure_manager() = default;
		
		void update();
		
		figure cur_figure = figure(UINT32_MAX);
		
		game* the_game = nullptr;
		uint32_t state = 0; // 0 - WAIT_SPWN, 1 - FREEZE, 2 - FALL.
		clock_t ticks = NO_SPAWN_TICKS;
};

class input_manager
{
	public:
		input_manager(): move_horizontal(0), move_rotational(0), force_fall(false) {}
		void process_input();
		
		int32_t move_horizontal;
		int32_t move_rotational;
		bool force_fall;
		game* the_game;
};

class game
{
	public:
		game();
		~game();
		
		void update_game();
		void process_input();
		void render();
		void check_dead();
		bool should_run() {return state != 2;}
		bool is_running() {return state == 0;}
		bool is_paused() {return state == 1;}
		bool is_stopped() {return state == 2;}
		bool is_dead() {return state == 3;}
		void set_running() {state = 0;}
		void set_paused() {state = 1;}
		void set_stopped() {state = 2;}
		void set_dead() {state = 3;}
	private:
		uint32_t state; // 0: RUNNING, 1: PAUSED, 2: STOPPED, 3: DEAD.
		vector<vector<char_data>> game_field;
		
		uint32_t ticks_count = 0;
		uint32_t log_ticks_count = 4;
		uint32_t user_score = 0;
		
		input_manager      input_mgr;
		cur_figure_manager cur_figure_mgr;
		
		friend cur_figure_manager;
		friend input_manager;
};

#endif
