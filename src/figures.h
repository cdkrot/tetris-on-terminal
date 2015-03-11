#ifndef __FIGURES_H

#define __FIGURES_H

#include <utility>
#include <vector>
#include "char_data.h"
#include "stdstyle.h"

using std::vector;
using std::pair;

struct transform
{
	int32_t dx;
	int32_t dy;
	// describes which cells we need to swap.
	// first in pair is src cell, the cell, in which was in figure on prev step.
	// and it is garanteed, that it exists.
	// [this is an assertion, the whole code makes]
	// second is dest cell, to swap with.
	// if it is empty, and exist, then transform will apply ok, otherwise not.
	// in both pairs, first x, second y.
	vector<pair<pair<int32_t, int32_t>, pair<int32_t, int32_t>>> transf_data;
	
	static transform zero_transform();
};


struct figure
{
	figure(uint32_t type): type(type) {};
	uint32_t x;
	uint32_t y;
	
	// tries to move on specified direction
	// returns true if no collide happened.
	void move_x_or_collide(int32_t dx, vector<vector<char_data>>& game_field);
	bool move_y_or_collide(vector<vector<char_data>>& game_field);
	bool rotate_bkw(vector<vector<char_data>>& game_field);
	bool rotate_fwd(vector<vector<char_data>>& game_field);
	
	
	void spawn(vector<vector<char_data>>& game_field);
	private:
	// returns true if applied succesfully.
	bool apply_transform(const transform& t, vector<vector<char_data>>& game_field);
	uint32_t type;
};

figure get_new_rand_figure();


#endif