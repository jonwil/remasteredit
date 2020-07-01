#pragma once
#include <map>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlwapi.h>
#pragma warning(disable: 4458)
#include <gdiplus.h>
#pragma warning(default: 4458)
template <class Key, class Value, class Compare, class Allocator> bool TryGetValue(std::map<Key, Value, Compare, Allocator> const& theMap, typename std::map<Key, Value, Compare, Allocator>::key_type const& key, typename std::map<Key, Value, Compare, Allocator>::mapped_type& value)
{
	auto it = theMap.find(key);
	if (it == theMap.end())
	{
		return false;
	}
	value = it->second;
	return true;
}
enum FacingType
{
	FACING_NONE,
	FACING_NORTH,
	FACING_NORTHEAST,
	FACING_EAST,
	FACING_SOUTHEAST,
	FACING_SOUTH,
	FACING_SOUTHWEST,
	FACING_WEST,
	FACING_NORTHWEST,
	FACING_COUNT
};
class CellMetrics
{
public:
	static const FacingType AdjacentFacings[8];
	int Width;
	int Height;
	Gdiplus::Point TopLeft;
	Gdiplus::Size Size;
	Gdiplus::Rect Bounds;
	int Length;
	bool Contains(Gdiplus::Point location) const
	{
		if (location.X >= 0 && location.X < Width && location.Y >= 0)
		{
			return location.Y < Height;
		}
		return false;
	}
	bool Contains(int cell) const
	{
		if (cell >= 0)
		{
			return cell < Length;
		}
		return false;
	}
	CellMetrics(int width, int height) : Width(width), Height(height), TopLeft(0, 0), Size(Width, Height), Bounds(TopLeft, Size), Length(Width * Height)
	{
	}
	CellMetrics(Gdiplus::Size size) : Width(size.Width), Height(size.Height), TopLeft(0, 0), Size(Width, Height), Bounds(TopLeft, Size), Length(Width * Height)
	{
	}
	bool GetCell(Gdiplus::Point location, int& cell) const
	{
		cell = location.Y * Width + location.X;
		return Contains(location);
	}
	bool GetLocation(int cell, Gdiplus::Point& location) const
	{
		location = Gdiplus::Point(cell % Width, cell / Width);
		return Contains(cell);
	}
	bool Adjacent(Gdiplus::Point location, FacingType facing, Gdiplus::Point& adjacent) const
	{
		adjacent = location;
		switch (facing)
		{
		case FACING_NORTH:
			adjacent.Y--;
			break;
		case FACING_NORTHEAST:
			adjacent.X++;
			adjacent.Y--;
			break;
		case FACING_EAST:
			adjacent.X++;
			break;
		case FACING_SOUTHEAST:
			adjacent.X++;
			adjacent.Y++;
			break;
		case FACING_SOUTH:
			adjacent.Y++;
			break;
		case FACING_SOUTHWEST:
			adjacent.X--;
			adjacent.Y++;
			break;
		case FACING_WEST:
			adjacent.X--;
			break;
		case FACING_NORTHWEST:
			adjacent.X--;
			adjacent.Y--;
			break;
		}
		return Contains(adjacent);
	}
	bool Adjacent(int cell, FacingType facing, int& adjacent) const
	{
		Gdiplus::Point location;
		Gdiplus::Point adjacent2;
		if (!GetLocation(cell, location) || !Adjacent(location, facing, adjacent2))
		{
			adjacent = -1;
			return false;
		}
		return GetCell(adjacent2, adjacent);
	}
	void Clip(Gdiplus::Point &location) const
	{
		location.X = max(0, min(Width - 1, location.X));
		location.Y = max(0, min(Height - 1, location.Y));
	}
	void Clip(Gdiplus::Point &location, Gdiplus::Size margin) const
	{
		location.X = max(margin.Width, min(Width - margin.Width - 1, location.X));
		location.Y = max(margin.Height, min(Height - margin.Height - 1, location.Y));
	}
};
