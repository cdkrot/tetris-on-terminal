#ifndef __CHAR_DATA_H

#define __CHAR_DATA_H

#include <vector>
#include <string>
#include "./stdstyle.h"

enum color_t
{
	red, green, yellow, blue, magenta, cyan, white, brown
};

char get_random_possible_char();
color_t get_random_possible_color();
std::string get_all_possible_chars();
std::vector<color_t> get_all_possible_colors();

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
