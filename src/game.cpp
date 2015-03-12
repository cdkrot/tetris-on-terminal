#include "stdstyle.h"
#include <stdio.h>
#include <algorithm>
#include "game.h"
#include "terminal.h"
#include <ncurses.h>
#include <set>
#include <string>

using std::string;
using std::set;

game::game()
{
	is_running = true;
	game_field.resize(GAME_HEIGHT);
	for (vector<char_data>& line: game_field)
	{
		line.resize(GAME_WIDTH);
		std::fill(line.begin(), line.end(), char_data::space());
	}
	
	cur_figure_mgr.the_game = this;
	move_horizontal = 0;
	move_rotational = 0;
}

game::~game()
{
	printf("# Bye #\n");
}

bool game::should_run()
{
	return is_running;
}

void game::stop()
{
	is_running = false;
}

void game::update_game()
{
	if (is_dead)
		return;
	
	job_mgr.invoke();
	
	// check for lines we can destroy.
	// score is calculated by this system:
	//
	// if you destroyed N rows in one turn, the one row will affect score like this:
	// score += N * (num_diffrent_chars_in_this_row) * time_modifier.
	// time_modifier grows like logarithm.
	//
	
	ticks_count++;
	while (ticks_count / 40 > (1u << log_ticks_count))
		log_ticks_count++;
	
	if (cur_figure_mgr.state == 0)
	{
		auto check_row = [](const vector<char_data>& v) -> bool
		{
			for (char_data c: v)
				if (c.character == ' ')
					return false;
			return true;
		};
		auto calc_row = [](const vector<char_data>& b) -> uint32_t
		{
			set<char_data> s;
			for (char_data c: b)
				s.insert(c);
			return s.size();
		};
		uint32_t delta_score = 0;
		uint32_t n_multiplier = 0;
		for (uint32_t i = GAME_HEIGHT - 1;;)
		{
			if (check_row(game_field[i]))
			{
				delta_score += calc_row(game_field[i]);
				n_multiplier++;
				// flow up.
				if (i != 0)
					for (uint32_t j = i - 1;;)
					{
						std::swap(game_field[j], game_field[j + 1]); // this is O(1).
						if (j == 0)
							break;
						j--;
					}
				for (char_data& c: game_field[0])
					c = char_data::space();
			}
			
			if (i == 0)
				break;
			i--;
		}
		user_score += n_multiplier * delta_score * log_ticks_count;
	}
	
	cur_figure_mgr.update();
}

void game::check_dead()
{
	for (uint32_t i = 0; i < GAME_HEIGHT - GAME_REAL_HEIGHT; i++)
		for (auto c: game_field[i])
			if (c.character != ' ')
			{
				is_dead = true;
				return;
			}
}

void game::process_input()
{
	int ch;
	force_fall = false;
	while (true)
	{
		ch = getch();
		if (ch == ERR)
			break; // no input yet.
		if (ch == 'q' or ch == 'Q')
			this->stop();
		if (ch == KEY_LEFT || ch == 'a' || ch == 'A')
			move_horizontal += -1;
		if (ch == KEY_RIGHT || ch == 'd' || ch == 'D')
			move_horizontal += +1;
		if (ch == KEY_DOWN || ch == 'z' || ch == 'Z')
			move_rotational += -1;
		if (ch == KEY_UP || ch == 'x' || ch == 'X')
			move_rotational += +1;
	}
}

void game::render()
{
	terminal_reset();
	terminal_put_string("Tetris On Terminal\n");
	terminal_put_string("           By cdkrot\n");
	terminal_put_string("=====================\n");
	const std::string dat[4] = {"00", "25", "50", "75"};
	std::string score_string = std::to_string(user_score / 4) + "." + dat[user_score % 4];
	std::string multipl_string = std::to_string(log_ticks_count / 4) + "." + dat[log_ticks_count % 4];
	terminal_put_string("Your score: " + score_string + ", multiplier: " + multipl_string
		+ "\n", (is_dead) ? color_t::red : color_t::white);
	
	// render here
	// TODO:
	for (uint32_t i = 0; i != GAME_HEIGHT; i++)
	{
		if (i >= GAME_HEIGHT - GAME_REAL_HEIGHT)
			terminal_put_char('#', is_dead ? color_t::red : color_t::white);
		else
			terminal_put_char(' ');
		
		for (uint32_t j = 0; j != GAME_WIDTH; j++)
			terminal_put_char(game_field[i][j].character, game_field[i][j].color);
		
		if (i >= GAME_HEIGHT - GAME_REAL_HEIGHT)
			terminal_put_string("#\n", is_dead ? color_t::red : color_t::white);
		else
			terminal_put_string("\n");
	}
	for (uint32_t j = 0; j != GAME_WIDTH + 2; j++)
		terminal_put_char('#', is_dead ? color_t::red : color_t::white);
	terminal_flush();
}

void cur_figure_manager::update()
{
	if (state == 0) // NO_SPAWN
	{
		if (ticks != 0)
			ticks--;
		else
		{
			the_game->check_dead();
			if (the_game->is_dead)
				return;
			state = 1;
			cur_figure = get_new_rand_figure();
			cur_figure.spawn(the_game->game_field);
			ticks = FREEZE_TICKS;
		}
	}
	else 
	{
		if (the_game->move_horizontal != 0)
		{
			cur_figure.move_x_or_collide(the_game->move_horizontal, the_game->game_field);
			the_game->move_horizontal = 0;
		}
		while (the_game->move_rotational < 0)
		{
			cur_figure.rotate_bkw(the_game->game_field);
			the_game->move_rotational++;
		}
		while (the_game->move_rotational > 0)
		{
			cur_figure.rotate_fwd(the_game->game_field);
			the_game->move_rotational--;
		}
		
		if (state == 1) // FREEZE.
		{
			if (ticks != 0)
				ticks--;
			else
				state = 2;
		}
		else // FALL.
		{
			if (ticks != 0)
				ticks--;
			else
			{
				if (cur_figure.move_y_or_collide(the_game->game_field) == false) // collided.
				{
					ticks = NO_SPAWN_TICKS;
					state = 0;
				}
				else
					ticks = TICKS_PER_FALL;
			}
		}
	}
}
