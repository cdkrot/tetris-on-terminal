#ifndef __GAME_H

#define __GAME_H

#include "stdstyle.h"
#include "char_data.h"
#include "figures.h"
#include "jobs.h"
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

class game
{
	public:
		game();
		~game();
		
		bool should_run();
		void stop();
		void update_game();
		void process_input();
		void render();
		void check_dead();
	private:
		bool is_running;
		vector<vector<char_data>> game_field;
		job_manager job_mgr;
		
		int32_t move_horizontal;
		int32_t move_rotational;
		bool force_fall;
		
		bool is_dead = false;
		uint32_t ticks_count = 0;
		uint32_t log_ticks_count = 4;
		uint32_t user_score = 0;
		
		cur_figure_manager cur_figure_mgr;
		
		friend cur_figure_manager;
};

#endif
