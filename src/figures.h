#ifndef __FIGURES_H

#define __FIGURES_H

#include <utility>
#include <vector>
#include "char_data.h"
#include "stdstyle.h"

using std::vector;
using std::pair;

struct coord_t
{
	int32_t x;
	int32_t y;
	
	bool operator<(const coord_t other) const
	{
		return x < other.x or (x == other.x and y < other.y);
	}
};


struct transform
{
	int32_t dx;
	int32_t dy;
	
	// describes src cells to dissapear and dst_cells to appear.
	vector<coord_t> src_cells;
	vector<coord_t> dst_cells;
	
	static transform zero_transform();
};

typedef std::vector<transform> multitransform;

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
	bool apply_transform(const multitransform& t, vector<vector<char_data>>& game_field);
	uint32_t type;
};

figure get_new_rand_figure();


#endif