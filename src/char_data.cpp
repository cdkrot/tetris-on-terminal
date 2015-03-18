#include "stdstyle.h"
#include "char_data.h"



char get_random_possible_char()
{
	const char* str = "X#@Z%&+O";
	return str[rand() % 8];
}

color_t get_random_possible_color()
{
	// sorry, i don't like electronic blue color.
	color_t dat[] = {color_t::red, color_t::green, color_t::yellow, color_t::magenta, color_t::cyan, color_t::white, color_t::brown};
	return dat[rand() % 7];
}
