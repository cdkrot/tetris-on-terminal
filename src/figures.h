#ifndef __FIGURES_H

#define __FIGURES_H

#include <utility>
#include <vector>
#include "./util/char_data.h"
#include "./util/coord.h"
#include "./stdstyle.h"


using std::vector;
using std::pair;

struct transform
{
	int32_t dx;
	int32_t dy;
	
	// describes src cells to dissapear and dst_cells to appear.
	vector<coord_t> src_cells;
	vector<coord_t> dst_cells;
	
	bool operator==(const transform& other) const
	{
		return (dx == other.dx) and (dy == other.dy) and (src_cells == other.src_cells) and (dst_cells == other.dst_cells);
	}
	
	bool operator!=(const transform& other) const
	{
		return not (*this == other);
	}
	
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
	
	uint32_t get_type(){return type;}
	void spawn(vector<vector<char_data>>& game_field);
	private:
	// returns true if applied succesfully.
	bool apply_transform(const transform& t, vector<vector<char_data>>& game_field);
	bool apply_transform(const multitransform& t, vector<vector<char_data>>& game_field);
	uint32_t type;
};

struct figure_type
{
	vector<coord_t> coords;
	bool antigravity_protection;
	vector<coord_t> rotation_points;
};

figure get_new_rand_figure();
figure_type get_figure_type_info(uint32_t type);

#endif