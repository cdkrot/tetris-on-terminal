#include <ncurses.h>
#include <memory>
#include <string>
#include "game.h"
#include "terminal.h"
#include "main_menu.h"
#include "stdstyle.h"


bool main_menu::should_run()
{
	return is_running;
}

void main_menu::update_game(){}

void main_menu::process_input()
{
	while (true)
	{
		int ch = getch();
		if (ch == ERR)
			break;
		if (ch == KEY_DOWN)
			sel = (sel + 1) % 6;
		if (ch == KEY_UP)
			sel = (sel + 5) % 6;
		if (ch == '\n' || ch == ' ')
		{
			if (sel == 0)
				mgr->push_screen(std::shared_ptr<iscreen>(new game(normal)));
			if (sel == 1)
				mgr->push_screen(std::shared_ptr<iscreen>(new game(hard)));
			if (sel == 2)
				mgr->push_screen(std::shared_ptr<iscreen>(new game(rainbow)));
			if (sel == 3)
				;//...
			if (sel == 4)
				;//...
			if (sel == 5)
				is_running = false;
		}
	}
}

void main_menu::render()
{
	terminal_reset();
	terminal_put_string("Tetris On Terminal v2.0\n");
	terminal_put_string("           By cdkrot\n");
	terminal_put_string("=====================\n");
	terminal_put_string("\nMain Menu\n", color_t::yellow);
	terminal_put_string("[");
	terminal_put_string("Play", (sel <= 2) ? color_t::yellow : color_t::white);
	terminal_put_string("]\n");
	
	std::string dat[3] = {"Normal", "Hard", "Rainbow mode"};
	for (uint32_t i = 0; i < 3; i++)
	{
		terminal_put_string("  [");
		terminal_put_string(dat[i], (sel == i) ? color_t::yellow: color_t::white);
		terminal_put_string("]\n");
	}
	
	std::string dat2[3] = {"Controls", "Credits", "Quit"};
	for (uint32_t i = 0; i < 3; i++)
	{
		terminal_put_string("[");
		color_t bright_color = (i != 3) ? color_t::yellow : color_t::red;
		terminal_put_string(dat2[i], (sel == i + 3) ? bright_color: color_t::white);
		terminal_put_string("]\n");
	}
	
}