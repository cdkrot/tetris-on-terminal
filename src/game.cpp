#include "stdstyle.h"
#include <stdio.h>
#include <algorithm>
#include "game.h"
#include "terminal.h"
#include <ncurses.h>
#include <set>
#include <string>
#include <map>

using std::map;
using std::string;
using std::set;
using std::max;

game::game()
{
	state = 0; // running.
	game_field.resize(GAME_HEIGHT);
	for (vector<char_data>& line: game_field)
	{
		line.resize(GAME_WIDTH);
		std::fill(line.begin(), line.end(), char_data::space());
	}
	
	cur_figure_mgr.the_game = this;
	input_mgr.the_game = this;
}

game::~game()
{
	printf("# Bye #\n");
}


void game::update_game()
{
	if (!is_running())
		return;
	
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
				set_dead();
				return;
			}
}

void game::process_input()
{
	input_mgr.process_input();
}

void input_manager::process_input()
{
	int ch;
	while (true)
	{
		ch = getch();
		if (ch == ERR)
			break; // no input yet.
		if (ch == 'q' or ch == 'Q')
			the_game->set_stopped();
		if (ch == ' ')
		{
			if (the_game->is_running())
				the_game->set_paused();
			else if (the_game->is_paused())
				the_game->set_running();
			continue;
		}
		if (!the_game->is_running())
			continue;
		if (ch == '\n')
			force_fall = true;
		if (ch == KEY_LEFT || ch == 'a' || ch == 'A')
			move_horizontal += -1;
		if (ch == KEY_RIGHT || ch == 'd' || ch == 'D')
			move_horizontal += +1;
		if (ch == KEY_DOWN || ch == 's' || ch == 'S')
			move_rotational += -1;
		if (ch == KEY_UP || ch == 'w' || ch == 'W')
			move_rotational += +1;
	}
}

void game::render()
{
	terminal_reset();
	terminal_put_string("Tetris On Terminal\n");
	terminal_put_string("           By cdkrot\n");
	terminal_put_string("=====================\n");
	terminal_put_string("Controls: arrow keys or wasd to move or rotate, space to pause, <ENTER> to force fall, q to quit.\n");
	const std::string dat[4] = {"00", "25", "50", "75"};
	std::string score_string = std::to_string(user_score / 4) + "." + dat[user_score % 4];
	std::string multipl_string = std::to_string(log_ticks_count / 4) + "." + dat[log_ticks_count % 4];
	std::string statuses[4] = {"RUNNING", "PAUSED", "STOPPED", "DEAD"};
	terminal_put_string("Your score: " + score_string + ", multiplier: " + multipl_string + " [" + statuses[state] + "]"
		+ "\n", is_dead() ? color_t::red : color_t::white);
	
	// render here
	map<uint32_t, uint32_t> the_map;
	color_t fig_color;
	if (cur_figure_mgr.state != 0)
	{
		auto fig = cur_figure_mgr.cur_figure;
		auto fig_img = get_type_image(fig.get_type());
		for (auto crd: fig_img)
			if (the_map.find(fig.x + crd.x) == the_map.end())
				the_map.emplace(fig.x + crd.x, fig.y + crd.y);
			else
				the_map[fig.x + crd.x] = max(the_map[fig.x + crd.x], fig.y + crd.y); // the deepest.
		fig_color = game_field[fig.y + fig_img[0].y][fig.x + fig_img[0].x].color;
	}
	for (uint32_t i = 0; i != GAME_HEIGHT; i++)
	{
		if (i >= GAME_HEIGHT - GAME_REAL_HEIGHT)
			terminal_put_char('#', is_dead() ? color_t::red : color_t::white);
		else
			terminal_put_char(' ');
		
		for (uint32_t j = 0; j != GAME_WIDTH; j++)
		{
			if (game_field[i][j].character == ' ' and the_map.find(j) != the_map.end() and the_map[j] < i)
				terminal_put_char('.', fig_color);
			else
			{
				if (game_field[i][j].character != ' ' and the_map.find(j) != the_map.end() and the_map[j] < i)
					the_map.erase(the_map.find(j));
				terminal_put_char(game_field[i][j].character, game_field[i][j].color);
			}
		}
		
		if (i >= GAME_HEIGHT - GAME_REAL_HEIGHT)
			terminal_put_string("#\n", is_dead() ? color_t::red : color_t::white);
		else
			terminal_put_string("\n");
	}
	for (uint32_t j = 0; j != GAME_WIDTH + 2; j++)
		terminal_put_char('#', is_dead() ? color_t::red : color_t::white);
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
			if (the_game->is_dead())
				return;
			state = 1;
			cur_figure = get_new_rand_figure();
			cur_figure.spawn(the_game->game_field);
			ticks = FREEZE_TICKS;
		}
		the_game->input_mgr.force_fall = false;
	}
	else 
	{
		if (the_game->input_mgr.move_horizontal != 0)
		{
			cur_figure.move_x_or_collide(the_game->input_mgr.move_horizontal, the_game->game_field);
			the_game->input_mgr.move_horizontal = 0;
		}
		while (the_game->input_mgr.move_rotational < 0)
		{
			cur_figure.rotate_bkw(the_game->game_field);
			the_game->input_mgr.move_rotational++;
		}
		while (the_game->input_mgr.move_rotational > 0)
		{
			cur_figure.rotate_fwd(the_game->game_field);
			the_game->input_mgr.move_rotational--;
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
				if (the_game->input_mgr.force_fall)
				{
					while (cur_figure.move_y_or_collide(the_game->game_field) == true){}
					ticks = NO_SPAWN_TICKS;
					state = 0;
					the_game->input_mgr.force_fall = false;
				}
				
				else if (cur_figure.move_y_or_collide(the_game->game_field) == false) // collided.
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
