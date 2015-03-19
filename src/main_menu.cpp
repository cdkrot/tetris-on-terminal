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
		if (ch == KEY_DOWN || ch == 's' || ch == 'S')
			sel = (sel + 1) % 6;
		if (ch == KEY_UP || ch == 'w' || ch == 'W')
			sel = (sel + 5) % 6;
		if (ch == '\n' || ch == ' ')
		{
			if (sel == 0)
				mgr->push_screen(std::shared_ptr<iscreen>(new game(normal)));
			if (sel == 1)
				mgr->push_screen(std::shared_ptr<iscreen>(new game(hard)));
			if (sel == 2)
				{}// rainbow not supported yet.// mgr->push_screen(std::shared_ptr<iscreen>(new game(rainbow)));
			if (sel == 3)
				mgr->push_screen(std::shared_ptr<iscreen>(new menu_controls(mgr)));
			if (sel == 4)
				mgr->push_screen(std::shared_ptr<iscreen>(new menu_credits(mgr)));
			if (sel == 5)
				is_running = false;
		}
		if (ch == 'q' || ch == 'Q')
			is_running = false;
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
		color_t bright_color = (i != 2) ? color_t::yellow : color_t::red;
		terminal_put_string(dat2[i], (sel == i + 3) ? bright_color: color_t::white);
		terminal_put_string("]\n");
	}
}

bool menu_credits::should_run()
{
	return is_running;
}

void menu_credits::update_game(){}

void menu_credits::process_input()
{
	while (true)
	{
		int ch = getch();
		if (ch == ERR)
			break;
		is_running = false;
	}
}

void menu_credits::render()
{
	terminal_reset();
	terminal_put_string("Tetris On Terminal v2.0\n");
	terminal_put_string("           By cdkrot\n");
	terminal_put_string("=====================\n");
	terminal_put_string("\nCredits\n", color_t::yellow);
	terminal_put_string("Copyright (C) 2015 Sayutin Dmitry (aka cdkrot).\n");
	terminal_put_string("https://github.com/cdkrot/tetris-on-terminal\n\n");
	
	terminal_put_string(std::string("") + 
		"This Program is free  software; you can redistribute it and/or modify it under the\n" + 
		"terms of the GNU General Public License as published by the\n" + 
		"Free Software Foundation; either version 3, or (at your option)\n" +
		"any later version.\n\n");
	terminal_put_string(std::string("") + 
		"This program is distributed in the hope that it will be useful,\n" + 
		"but WITHOUT ANY WARRANTY; without even the implied warranty of\n" + 
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n" + 
		"GNU General Public License for more details.\n\n");
	
	terminal_put_string(std::string("") + 
		"You should have received a copy of the GNU General Public License\n" + 
		"If not, see <http://www.gnu.org/licenses/>.\n");
}

bool menu_controls::should_run()
{
	return is_running;
}

void menu_controls::update_game(){}

void menu_controls::process_input()
{
	while (true)
	{
		int ch = getch();
		if (ch == ERR)
			break;
		is_running = false;
	}
}

void menu_controls::render()
{
	terminal_reset();
	terminal_put_string("Tetris On Terminal v2.0\n");
	terminal_put_string("           By cdkrot\n");
	terminal_put_string("=====================\n");
	terminal_put_string("\nControls\n", color_t::yellow);
	terminal_put_string("<-, ->, A, D to move left or right\n");
	terminal_put_string("up, down, W, S, to rotate\n");
	terminal_put_string("Enter to force figure to fall\n");
	terminal_put_string("Space to pause the game\n");
	terminal_put_string("Q to quit\n");
	terminal_put_string("\nPress any button to continue.\n");
}