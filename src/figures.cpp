#include "stdstyle.h"
#include "figures.h"
#include <utility>
#include <map>
#include <algorithm>

using std::min;
using std::map;
using std::pair;
using std::make_pair;

#define NUM_TYPES 16

figure get_new_rand_figure()
{	
	figure f(4 * (rand() / (NUM_TYPES / 4)) + rand() % 3);
	f.x = f.y = 0;
	return f;
}

transform transform::zero_transform()
{
	return transform{0, 0, {}, {}};
}

transform compute_move_transform(int32_t dx, int32_t dy, vector<coord_t> data)
{
	map<coord_t, bool> the_map; // flag: is_dst?
	auto smart_insert = [&the_map](coord_t val, bool flag)
	{
		auto it = the_map.find(val);
		if (it == the_map.end())
			the_map.emplace(val, flag);
		else
			the_map.erase(it);
	};
	
	for (coord_t c: data)
	{
		smart_insert(c, false);
		smart_insert({c.x + dx, c.y + dy}, true);
	}
	
	transform res = {dx, dy, {}, {}};
	for (auto v: the_map)
		if (v.second == 0)
			res.src_cells.push_back(v.first);
		else
			res.dst_cells.push_back(v.first);
	return res;
}

transform compute_rotate_transformation(bool rotate_fwd, coord_t rot_cent, vector<coord_t> data)
{
	// compute delta center automagically.
	int32_t new_x = INT32_MAX;
	int32_t new_y = INT32_MAX;
	
	map<coord_t, bool> the_map; // flag: is_dst?
	auto smart_insert = [&the_map, &new_x, &new_y](coord_t val, bool flag)
	{
		auto it = the_map.find(val);
		if (it == the_map.end())
			the_map.emplace(val, flag);
		else
			the_map.erase(it);
		if (flag)
			new_x = min(new_x, val.x), new_y = min(new_y, val.y);
	};
	
	for (coord_t c: data)
	{
		smart_insert(c, false);
		coord_t delta_center = {c.x - rot_cent.x, c.y - rot_cent.y};
		if (rotate_fwd) // clockwise
			smart_insert({rot_cent.x - delta_center.y, rot_cent.y + delta_center.x}, true);
		else
			smart_insert({rot_cent.x + delta_center.y, rot_cent.y - delta_center.x}, true);
	}
	
	transform res = {new_x, new_y, {}, {}};
	
	for (auto v: the_map)
		if (v.second == 0)
			res.src_cells.push_back(v.first);
		else
			res.dst_cells.push_back(v.first);
	
	return res;
}

vector<coord_t> get_type_image(uint32_t type)
{
	static vector<coord_t> dat[] = {
		{{0, 0}, {0, 1}, {1, 0}, {1, 1}}, // 00
		{{0, 0}, {0, 1}, {1, 0}, {1, 1}}, // 01
		{{0, 0}, {0, 1}, {1, 0}, {1, 1}}, // 02
		{{0, 0}, {0, 1}, {1, 0}, {1, 1}}, // 03
		{{0, 0}, {1, 0}, {1, 1}, {2, 0}}, // 04
		{{0, 1}, {1, 0}, {1, 1}, {1, 2}}, // 05
		{{0, 1}, {1, 0}, {1, 1}, {2, 1}}, // 06
		{{0, 0}, {0, 1}, {0, 2}, {1, 1}}, // 07
		{{0, 0}, {0, 1}, {0, 2}, {1, 2}}, // 08
		{{0, 0}, {0, 1}, {1, 0}, {2, 0}}, // 09
		{{0, 0}, {1, 0}, {1, 1}, {1, 2}}, // 10
		{{0, 1}, {1, 1}, {2, 0}, {2, 1}}, // 11
		{{0, 2}, {1, 0}, {1, 1}, {1, 2}}, // 12
		{{0, 0}, {0, 1}, {1, 1}, {2, 1}}, // 13
		{{0, 0}, {0, 1}, {0, 2}, {1, 0}}, // 14
		{{0, 0}, {1, 0}, {2, 0}, {2, 1}}  // 15
	};
	return dat[type];
}

vector<coord_t> get_rotation_points_by_type(uint32_t type)
{
	static vector<coord_t> dat[] = {
		{}, // 0
		{}, // 1
		{}, // 2
		{}, // 3
		{{1, 0}}, // 4
		{{1, 1}}, // 5
		{{0, 1}}, // 6
		{{1, 1}}, // 7
		
		{{0, 1}}, // 8
		{{1, 0}}, // 9
		{{1, 1}}, // 10
		{{1, 1}}, // 11
		
		{{1, 1}}, // 12
		{{1, 1}}, // 13
		{{0, 1}}, // 14
		{{1, 0}}
	};
	return dat[type];
}

multitransform get_transform_by_type_and_id(uint32_t type, uint32_t id)
{
	// we could handle it all in on array, but it is much simpler to maintain
	// it's data when they are separate.
	static bool initialized = false;
	static multitransform MOV_LFT[NUM_TYPES];
	static multitransform MOV_RGH[NUM_TYPES];
	static multitransform MOV_DWN[NUM_TYPES];
	static multitransform MOV_BCK[NUM_TYPES];
	static multitransform MOV_FWD[NUM_TYPES];
	
	if (!initialized)
	{
		// Welcome to the Legion of Chaos!
		// see for the previous versions of this place,
		// really, it is worth it.
		for (uint32_t i = 0; i != NUM_TYPES; i++)
		{
			MOV_DWN[i] = {compute_move_transform( 0, 1, get_type_image(i))};
			MOV_LFT[i] = {compute_move_transform(-1, 0, get_type_image(i))};
			MOV_RGH[i] = {compute_move_transform(+1, 0, get_type_image(i))};
			
			auto rot_points = get_rotation_points_by_type(i);
			if (rot_points.empty())
				MOV_FWD[i] = MOV_BCK[i] = {transform::zero_transform()};
			else
			{
				MOV_FWD[i] = MOV_BCK[i] = {};
				for (auto e: rot_points)
				{
					MOV_FWD[i].push_back(compute_rotate_transformation(1, e, get_type_image(i)));
					MOV_BCK[i].push_back(compute_rotate_transformation(0, e, get_type_image(i)));
				}
			}
		}
		
		initialized = true;
	}
	
	if (id == 0)
		return MOV_FWD[type];
	if (id == 1)
		return MOV_BCK[type];
	if (id == 2)
		return MOV_DWN[type];
	if (id == 3)
		return MOV_LFT[type];
	if (id == 4)
		return MOV_RGH[type];
	assert(false);
}

void figure::spawn(vector<vector<char_data>>& game_field)
{	
	char_data ch = {get_random_possible_char(), get_random_possible_color()};
	
	for (coord_t c: get_type_image(type))
		game_field[c.y][c.x] = ch;
}

bool figure::apply_transform(const transform& t, vector<vector<char_data>>& game_field)
{
	// try apply atomic transaction
	assert(t.src_cells.size() == t.dst_cells.size());
	if (t.src_cells.size() != 0)
	{
		char_data ch = game_field[this->y + t.src_cells[0].y][this->x + t.src_cells[0].x];
		for (coord_t crd: t.dst_cells)
		{
			int32_t y = this->y;
			int32_t x = this->x;
			if (y + crd.y < 0 or x + crd.x < 0)
				return false;
			if (y + crd.y >= GAME_HEIGHT or x + crd.x >= GAME_WIDTH)
				return false;
			if (game_field[y + crd.y][x + crd.x].character != ' ')
				return false;
		}
		for (coord_t crd: t.src_cells)
			game_field[this->y + crd.y][this->x + crd.x] = char_data::space();
		for (coord_t crd: t.dst_cells)
			game_field[this->y + crd.y][this->x + crd.x] = ch;
	}
	
	this->x += t.dx;
	this->y += t.dy;
	return true;
}

bool figure::apply_transform(const multitransform& t, vector<vector<char_data>>& game_field)
{
	for (const transform& transf: t)
		if (apply_transform(transf, game_field) == true)
			return true;
	return false;
}


void figure::move_x_or_collide(int32_t dx, vector<vector<char_data>>& game_field)
{
	
	if (dx >= +1)
		for (; dx != 0; dx--)
			apply_transform(get_transform_by_type_and_id(type, 4), game_field);
	if (dx <= -1)
		for (; dx != 0; dx++)
			apply_transform(get_transform_by_type_and_id(type, 3), game_field);
}

bool figure::move_y_or_collide(vector<vector<char_data>>& game_field)
{
	return apply_transform(get_transform_by_type_and_id(type, 2), game_field);
}

bool figure::rotate_fwd(vector<vector<char_data>>& game_field)
{
	bool res = apply_transform(get_transform_by_type_and_id(type, 0), game_field);
	if (res)
		type = ((type + 1) % 4 == 0) ? (type - 3) : (type + 1);
	return res;
}

bool figure::rotate_bkw(vector<vector<char_data>>& game_field)
{
	bool res = apply_transform(get_transform_by_type_and_id(type, 1), game_field);
	if (res)
		type = (type % 4 == 0) ? (type + 3) : (type - 1);
	return res;
}
