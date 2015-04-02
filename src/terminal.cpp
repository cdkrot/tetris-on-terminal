#include "terminal.h"

#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include "./stdstyle.h"

decltype(COLOR_PAIR(0)) colort_to_colorpair(color_t t)
{
	// red, green, yellow, blue, magenta, cyan, white
	if (t == color_t::red)
		return COLOR_PAIR(1);
	if (t == color_t::green)
		return COLOR_PAIR(2);
	if (t == color_t::yellow || t == color_t::brown)
		return COLOR_PAIR(3);
	if (t == color_t::blue)
		return COLOR_PAIR(4);
	if (t == color_t::magenta)
		return COLOR_PAIR(5);
	if (t == color_t::cyan)
		return COLOR_PAIR(6);
	return COLOR_PAIR(7);
}

void terminal_init()
{
	initscr();
	keypad(stdscr, TRUE);
	raw();
	nodelay(stdscr, TRUE);
	noecho(); //Not display what user is printing.
	curs_set(0); //Hide cursor
	if (has_colors()) 
	{
		start_color();
		init_pair(1, COLOR_RED,     COLOR_BLACK);
		init_pair(2, COLOR_GREEN,   COLOR_BLACK);
		init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
		init_pair(4, COLOR_BLUE,    COLOR_BLACK);
		init_pair(5, COLOR_CYAN,    COLOR_BLACK);
		init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
		init_pair(7, COLOR_WHITE,   COLOR_BLACK);
	}
}

void terminal_reset()
{
	erase();
}

void terminal_put_string(std::string s)
{
	printw("%s", s.c_str());
}

void terminal_put_string(std::string str, color_t col)
{
	for (auto c: str)
		terminal_put_char(c, col);
}

void terminal_put_char(char c)
{
	addch(c);
}

void terminal_put_char(char ch, color_t col)
{
	if (col == color_t::yellow)
		attron(A_BOLD);
	addch(ch | colort_to_colorpair(col));
	if (col == color_t::yellow)
		attroff(A_BOLD);
}

void terminal_endline()
{
	addch('\n');
}

void terminal_flush()
{
	refresh();
}

void terminal_shutdown()
{
	endwin();
}

