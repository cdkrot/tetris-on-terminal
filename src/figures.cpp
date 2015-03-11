#include "stdstyle.h"
#include "figures.h"
#include <utility>

using std::pair;
using std::make_pair;

figure get_new_rand_figure()
{
	figure f(rand() % 4);
	f.x = f.y = 0; // TODO:.
	return f;
}

transform transform::zero_transform()
{
	return transform{0, 0, {}};
}


transform get_transform_by_type_and_id(uint32_t type, uint32_t id)
{
	// we could handle it all in on array, but it is much simpler to maintain
	// it's data when they are separate.
	static bool initialized = false;
	static transform ROT_FWD[4];
	static transform ROT_BCK[4];
	static transform MOV_DWN[4];
	static transform MOV_LFT[4];
	static transform MOV_RGH[4];
	
	if (!initialized)
	{
		// 0-3 is square.
		ROT_FWD[0] = ROT_FWD[1] = ROT_FWD[2] = ROT_FWD[3] = transform::zero_transform();
		ROT_BCK[0] = ROT_BCK[1] = ROT_BCK[2] = ROT_BCK[3] = transform::zero_transform();
		MOV_DWN[0] = MOV_DWN[1] = MOV_DWN[2] = MOV_DWN[3] =
			transform {0, 1, {make_pair(make_pair(0, 0), make_pair(0, 2)), make_pair(make_pair(1, 0), make_pair(1, 2))}};
		MOV_LFT[0] = MOV_LFT[1] = MOV_LFT[2] = MOV_LFT[3] = 
			transform {-1, 0, {make_pair(make_pair(1, 0), make_pair(-1, 0)), make_pair(make_pair(1, 1), make_pair(-1, 1))}};
		MOV_RGH[0] = MOV_RGH[1] = MOV_RGH[2] = MOV_RGH[3] = 
			transform {+1, 0, {make_pair(make_pair(0, 0), make_pair(2, 0)), make_pair(make_pair(0, 1), make_pair(2, 1))}};
			
		initialized = true;
	}
	
	if (id == 0)
		return ROT_FWD[type];
	if (id == 1)
		return ROT_BCK[type];
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
	if (type < 4)
	{
		game_field[x][y] = game_field[x + 1][y] = game_field[x][y + 1] = game_field[x + 1][y + 1] = 
			{get_random_possible_char(), get_random_possible_color()};
	}
	else
		assert(false); // TODO: and check todo above.
}

bool figure::apply_transform(const transform& t, vector<vector<char_data>>& game_field)
{
	// try apply atomic transaction
	size_t i = 0;
	for (; i < t.transf_data.size(); i++)
	{
		auto src_cell  = t.transf_data[i].first;
		auto dst_cell = t.transf_data[i].second;
		src_cell.first  += this->x;
		src_cell.second += this->y;
		dst_cell.first  += this->x;
		dst_cell.second += this->y;
		
		if (dst_cell.first < 0 or dst_cell.second < 0 or dst_cell.first >= GAME_WIDTH or dst_cell.second >= GAME_HEIGHT
			or game_field[dst_cell.second][dst_cell.first].character != ' ')
		{
			// all gone worse, we need to reverse the transaction.
			if (i == 0)
				return false;
			for (i--;;)
			{
				std::swap(game_field[t.transf_data[i].first.second + y][t.transf_data[i].first.first + x], game_field[t.transf_data[i].second.second + y][t.transf_data[i].second.first + x]);
				if (i == 0)
					return false;
				i--;
			}
		}
		std::swap(game_field[src_cell.second][src_cell.first], game_field[dst_cell.second][dst_cell.first]);
	}
	
	this->x += t.dx;
	this->y += t.dy;
	return true;
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
