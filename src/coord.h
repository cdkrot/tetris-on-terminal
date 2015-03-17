#ifndef COORD_H

#define COORD_H

struct coord_t
{
	int32_t x;
	int32_t y;
	
	bool operator<(const coord_t other) const
	{
		return x < other.x or (x == other.x and y < other.y);
	}
};

#endif