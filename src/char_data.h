#ifndef __CHAR_DATA_H

#define __CHAR_DATA_H

#include "stdstyle.h"

enum color_t
{
	red, green, yellow, blue, magenta, cyan, white
};

char get_random_possible_char();
color_t get_random_possible_color();

struct char_data
{
	char character;
	color_t color;
	
	bool operator<(const char_data other) const
	{
		return (character < other.character) or (character == other.character and color < other.color);
	}
	
	static char_data space()
	{
		char_data c = {' ', color_t::white};
		return c;
	}
};

#endif
