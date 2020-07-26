#pragma once
enum FacingType {
	FACING_NONE = -1,
	FACING_N,
	FACING_NE,
	FACING_E,
	FACING_SE,
	FACING_S,
	FACING_SW,
	FACING_W,
	FACING_NW,
	FACING_COUNT,
	FACING_FIRST = 0
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
		case FACING_N:
			adjacent.Y--;
			break;
		case FACING_NE:
			adjacent.X++;
			adjacent.Y--;
			break;
		case FACING_E:
			adjacent.X++;
			break;
		case FACING_SE:
			adjacent.X++;
			adjacent.Y++;
			break;
		case FACING_S:
			adjacent.Y++;
			break;
		case FACING_SW:
			adjacent.X--;
			adjacent.Y++;
			break;
		case FACING_W:
			adjacent.X--;
			break;
		case FACING_NW:
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
