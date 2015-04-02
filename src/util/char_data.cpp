#include "./stdstyle.h"
#include "./char_data.h"



char get_random_possible_char()
{
	return get_all_possible_chars()[rand() % get_all_possible_chars().size()];
}

std::string get_all_possible_chars()
{
	return "X#@Z%&+O";
}

color_t get_random_possible_color()
{
	return get_all_possible_colors()[rand() % get_all_possible_colors().size()];
}

std::vector<color_t> get_all_possible_colors()
{
	// sorry, i don't like electronic blue color.
	return {color_t::red, color_t::green, color_t::yellow, color_t::magenta, color_t::cyan, color_t::white, color_t::brown};
}
