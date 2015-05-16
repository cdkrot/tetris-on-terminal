#include <ncurses.h>
#include <set>
#include <string>
#include <map>
#include <stdio.h>
#include <algorithm>
#include "./game.h"
#include "./terminal.h"
#include "./stdstyle.h"

using std::map;
using std::string;
using std::set;
using std::max;

game::game(game_settings gm_settings): gm_settings(gm_settings), input_mgr(this), cur_figure_mgr(this), rainbow_feat(this)
{
	state = 0; // running.
	game_field.resize(GAME_HEIGHT);
	for (vector<char_data>& line: game_field)
	{
		line.resize(GAME_WIDTH);
		std::fill(line.begin(), line.end(), char_data::space());
	}
}

game::~game()
{
}


void game::update_game()
{
	if (!is_running())
		return;
	
	// check for lines we can destroy.
	// score is calculated by this system:
	//
	// if you destroyed N rows in one turn, the one row will affect score like this:
	// score += 2 ^ (N - 1) * (num_diffrent_chars_in_this_row) * figure_modifier.
	// figure_modifier grows like logarithm from count of figures.
	//
	
	if (cur_figure_mgr.is_wait_spwn())
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
		if (n_multiplier != 0)
			user_score += (1 << (n_multiplier - 1)) * delta_score * cur_figure_mgr.get_figure_modifier();
	}
	
	rainbow_feat.update();
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
	terminal_put_string(std::string("") + "Tetris On Terminal " + BUILD_VERSION + "\n");
	terminal_put_string("           By cdkrot\n");
	terminal_put_string("=====================\n");
	terminal_put_string("\nGame\n", color_t::yellow);

	const std::string dat[2] = {"00", "50"};
	std::string score_string = std::to_string(user_score / 2) + "." + dat[user_score % 2];
	std::string multipl_string = std::to_string(cur_figure_mgr.get_figure_modifier() / 2) + "."
		+ dat[cur_figure_mgr.get_figure_modifier() % 2];
	std::string statuses[4] = {"RUNNING", "PAUSED", "STOPPED", "DEAD"};
	terminal_put_string("Your score: " + score_string + ", multiplier: " + multipl_string + " [" + statuses[state] + "]"
		+ "\n", is_dead() ? color_t::red : color_t::white);
	
	// render here
	map<uint32_t, uint32_t> the_map;
	color_t fig_color;
	if (not cur_figure_mgr.is_wait_spwn())
	{
		auto fig = cur_figure_mgr.get_figure();
		auto figure_type = get_figure_type_info(fig.get_type());
		for (auto crd: figure_type.coords)
			if (the_map.find(fig.x + crd.x) == the_map.end())
				the_map.emplace(fig.x + crd.x, fig.y + crd.y);
			else
				the_map[fig.x + crd.x] = max(the_map[fig.x + crd.x], fig.y + crd.y); // the deepest.
		fig_color = game_field[fig.y + figure_type.coords[0].y][fig.x + figure_type.coords[0].x].color;
	}
	for (uint32_t i = 0; i != GAME_HEIGHT; i++)
	{
		if (i >= GAME_HEIGHT - GAME_REAL_HEIGHT)
			terminal_put_char('#', is_dead() ? color_t::red : color_t::white);
		else
			terminal_put_char(' ');
		
		for (uint32_t j = 0; j != GAME_WIDTH; j++)
		{
			if (gm_settings.ticks_per_rainbow == UINT32_MAX and game_field[i][j].character == ' ' and the_map.find(j) != the_map.end() and the_map[j] < i)
				terminal_put_char('.', fig_color);
			else
			{
				if (game_field[i][j].character != ' ' and the_map.find(j) != the_map.end() and the_map[j] < i)
					the_map.erase(the_map.find(j));
				auto e = rainbow_feat.transform(game_field, j, i);
				terminal_put_char(e.character, e.color);
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

void cur_figure_manager::set_freeze()
{
	state = 1;
	ticks = the_game->get_game_settings().freeze_ticks;
	cur_figure = get_new_rand_figure();
	cur_figure.spawn(the_game->game_field);
	figures_count++;
	if (figures_count > (1u << log_figures_count))
		log_figures_count++;
}

void cur_figure_manager::set_wait_spwn()
{
	state = 0;
	ticks = the_game->get_game_settings().no_spawn_ticks;
}

void cur_figure_manager::update()
{
	if (is_wait_spwn())
	{
		if (ticks != 0)
			ticks--;
		else
		{
			the_game->check_dead();
			if (the_game->is_dead())
				return;
			set_freeze();
		}
		the_game->input_mgr.move_horizontal = 0;
		the_game->input_mgr.move_rotational = 0;
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
		
		if (is_freeze()) // FREEZE.
		{
			if (ticks != 0)
				ticks--;
			else
				set_fall();
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
					set_wait_spwn();
				}
				
				else if (cur_figure.move_y_or_collide(the_game->game_field) == false) // collided.
					set_wait_spwn();
				else
					ticks = the_game->get_game_settings().ticks_per_fall;
			}
		}
	}
}

rainbow_feature::rainbow_feature(game* the_game): the_game(the_game)
{
	auto settings = the_game->get_game_settings();
	ticks_per_rainbow = settings.ticks_per_rainbow;
	if (settings.ticks_per_rainbow_rand_fact != 0)
		ticks_per_rainbow += rand() % settings.ticks_per_rainbow_rand_fact;
	ticks_per_invert  = settings.ticks_per_invert;
	if (settings.ticks_per_invert_rand_fact != 0)
		ticks_per_invert += rand() % settings.ticks_per_invert_rand_fact;
	in_invert = false;
	
	blackout = char_data{get_random_possible_char(), get_random_possible_color()};
	
	for (color_t col: get_all_possible_colors())
		for (char c: get_all_possible_chars())
		{
			possible_symbols.push_back(char_data{c, col});
			the_mapping[char_data{c, col}] = char_data{c, col};
		}
	
}

void rainbow_feature::update()
{
	auto settings = the_game->get_game_settings();
	
	if (ticks_per_rainbow == 0)
	{
		if (in_invert)
			blackout = char_data{get_random_possible_char(), get_random_possible_color()};
		
		std::random_shuffle(possible_symbols.begin(), possible_symbols.end());
		{
			auto it = the_mapping.begin();
			for (uint32_t i = 0; i != possible_symbols.size(); ++i, ++it)
				it->second = possible_symbols[i];
		}
		
		ticks_per_rainbow = settings.ticks_per_rainbow + rand() % settings.ticks_per_rainbow_rand_fact;
	}
	else
		ticks_per_rainbow--;
	
	if (ticks_per_invert == 0)
	{
		in_invert = !in_invert;
		ticks_per_invert = settings.ticks_per_invert  + rand() % settings.ticks_per_invert_rand_fact;
	}
	else
		ticks_per_invert--;
}

char_data rainbow_feature::transform(const vector<vector<char_data>>& game_field, uint32_t x, uint32_t y)
{
	if (in_invert)
		if (game_field[y][x].character == ' ')
			return blackout;
		else
			return char_data::space();
	else
		if (game_field[y][x].character == ' ')
			return game_field[y][x];
		else
			return the_mapping[game_field[y][x]];
}