#pragma once
#include <string>
#include <map>
#include <vector>
#include "cellmetrics.h"
#include "boolinq.h"
using namespace boolinq;
namespace Gdiplus
{
	class Graphics;
}
class TemplateType;
class OverlayType;
class SmudgeType;
class TerrainType;
template <class T> class CellChangedEventArgs
{
public:
	int Cell;
	Gdiplus::Point Location;
	T OldValue;
	T Value;
	CellChangedEventArgs(const CellMetrics *metrics, int cell, T oldValue, T value)
	{
		Cell = cell;
		metrics->GetLocation(cell, Location);
		OldValue = oldValue;
		Value = value;
	}
	CellChangedEventArgs(const CellMetrics *metrics, Gdiplus::Point location, T oldValue, T value)
	{
		Location = location;
		metrics->GetCell(location, Cell);
		OldValue = oldValue;
		Value = value;
	}
};
template <class T> class CellGrid
{
private:
	const CellMetrics* metrics;
	T* cells;
public:
	Gdiplus::Size size;
	int length;
	T Get(int x, int y)
	{
		return cells[(y * metrics->Width) + x];
	}
	void Set(int x, int y, T value)
	{
		int cell = (y * metrics->Width) + x;
		if (cells[cell].type != value.type || cells[cell].icon != value.icon)
		{
			T oldValue = cells[cell];
			cells[cell] = value;
			if (OnCellChanged)
			{
				OnCellChanged(CellChangedEventArgs<T>(metrics, Gdiplus::Point(x, y), oldValue, value));
			}
		}
	}
	T Get(Gdiplus::Point location)
	{
		return Get(location.X, location.Y);
	}
	void Set(Gdiplus::Point location, T value)
	{
		Set(location.X, location.Y, value);
	}
	T Get(int cell)
	{
		return Get(cell % metrics->Width, cell / metrics->Width);
	}
	void Set(int cell, T value)
	{
		return Set(cell % metrics->Width, cell / metrics->Width, value);
	}
	void (*OnCellChanged)(CellChangedEventArgs<T> args);
	void (*OnCleared)();
	CellGrid(CellMetrics* m) : metrics(m)
	{
		metrics = m;
		cells = new T[metrics->Length];
	}
	void Clear()
	{
		delete[] cells;
		cells = new T[metrics->Length];
		if (OnCleared)
		{
			OnCleared();
		}
	}
	T Adjacent(Gdiplus::Point location, FacingType facing)
	{
		Gdiplus::Point adjacent;
		if (!metrics->Adjacent(location, facing, adjacent))
		{
			return T();
		}
		return Get(adjacent);
	}
	T Adjacent(int cell, FacingType facing)
	{
		Gdiplus::Point location;
		if (!metrics->GetLocation(cell, location))
		{
			return T();
		}
		Gdiplus::Point adjacent;
		if (!metrics->Adjacent(location, facing, adjacent))
		{
			return T();
		}
		return Get(adjacent);
	}
	bool CopyTo(CellGrid<T> other)
	{
		if (metrics->Length != other.metrics->Length)
		{
			return false;
		}
		for (int i = 0; i < metrics->Length; i++)
		{
			other.Set(i,Get(i));
		}
		return true;
	}
	~CellGrid()
	{
		delete[] cells;
	}
};
template <class T> class OccupierEventArgs
{
public:
	int Cell;
	Gdiplus::Point Location;
	T Occupier;
	OccupierEventArgs(const CellMetrics *metrics, int cell, T occupier)
	{
		Cell = cell;
		metrics->GetLocation(cell, Location);
		Occupier = occupier;
	}
	OccupierEventArgs(const CellMetrics *metrics, Gdiplus::Point location, T occupier)
	{
		Location = location;
		metrics->GetCell(location, Cell);
		Occupier = occupier;
	}
};
template <class T> class OccupierSet
{
private:
	CellMetrics* metrics;
	std::map<T, Gdiplus::Point> occupiers;
	T* occupierCells;
public:
	T Get(Gdiplus::Point location)
	{
		return Get(location.X, location.Y);
	}
	T Get(int x, int y)
	{
		if (!Contains(x, y))
		{
			return T();
		}
		return occupierCells[(y * metrics->Width) + x];
	}
	T Get(int cell)
	{
		Gdiplus::Point location;
		if (!metrics->GetLocation(cell, location))
		{
			return T();
		}
		return Get(location);
	}
	Gdiplus::Point Get(T occupier)
	{
		if (occupiers.count(T))
		{
			return occupiers(occupier);
		}
		return Gdiplus::Point(-1,-1);
	}
	void (*OnOccupierAdded)(OccupierEventArgs<T> args);
	void (*OnOccupierRemoved)(OccupierEventArgs<T> args);
	void (*OnCleared)();
	OccupierSet(CellMetrics* m) : metrics(m), OnCleared(nullptr), OnOccupierAdded(nullptr), OnOccupierRemoved(nullptr)
	{
		occupierCells = new T[metrics->Length];
	}
	~OccupierSet()
	{
		delete[] occupierCells;
	}
	bool CanAdd(Gdiplus::Point location, T occupier, bool *occupyMask, int width, int height)
	{
		if (occupier || Contains(occupier))
		{
			return false;
		}
		auto array = GetOccupyPoints(location, occupyMask, width, height);
		return !from(array).any([&](Gdiplus::Point p) {return !Contains(p) || Get(p) == nullptr; });
	}
	bool CanAdd(int cell, T occupier, bool *occupyMask, int width, int height)
	{
		Gdiplus::Point location;
		if (!metrics->GetLocation(cell, location))
		{
			return false;
		}
		return CanAdd(location, occupier, occupyMask, width, height);
	}
	bool CanAdd(int cell, T occupier)
	{
		if (occupier == nullptr)
		{
			return false;
		}
		return CanAdd(cell, occupier, occupier.OccupyMask, occupier.Width, occupier.Height);
	}
	bool Add(Gdiplus::Point location, T occupier, bool *occupyMask, int width, int height)
	{
		if (!DoAdd(location, occupier, occupyMask, width, height))
		{
			return false;
		}
		if (OnOccupierAdded)
		{
			OnOccupierAdded(OccupierEventArgs<T>(metrics, location, occupier));
		}
		return true;
	}
	bool Add(int cell, T occupier, bool *occupyMask, int width, int height)
	{
		Gdiplus::Point location;
		if (!metrics->GetLocation(cell, location))
		{
			return false;
		}
		return Add(location, occupier, occupyMask, width, height);
	}
	bool Add(Gdiplus::Point location, T occupier)
	{
		if (occupier == nullptr)
		{
			return false;
		}
		return Add(location, occupier, occupier->OccupyMask, occupier->Width, occupier->Height);
	}
	bool Add(int cell, T occupier)
	{
		if (occupier == nullptr)
		{
			return false;
		}
		return Add(cell, occupier, occupier->OccupyMask, occupier->Width, occupier->Height);
	}
	void Clear()
	{
		occupiers.clear();
		delete[] occupierCells;
		occupierCells = new T[metrics->Length];
		OnCleared();
	}
	bool Contains(int x, int y)
	{
		if (x >= 0 && x < metrics->Height && y >= 0)
		{
			return y < metrics->Width;
		}
		return false;
	}
	bool Contains(Gdiplus::Point location)
	{
		return Contains(location.X, location.Y);
	}
	bool Contains(T occupier)
	{
		return occupiers.count(occupier);
	}
	bool Remove(T occupier)
	{
		Gdiplus::Point point = Get(occupier);
		if (point.X == -1 && point.Y == -1)
		{
			return false;
		}
		if (!DoRemove(occupier))
		{
			return false;
		}
		if (OnOccupierRemoved)
		{
			OnOccupierRemoved(OccupierEventArgs<T>(metrics, point.Value, occupier));
		}
		return true;
	}
	bool Remove(Gdiplus::Point location)
	{
		return Remove(Get(location));
	}
	std::vector<std::pair<T, Gdiplus::Point>> GetOccupiers(const type_info& t)
	{
		std::vector<std::pair<T, Gdiplus::Point>> vector;
		for (auto i : occupiers)
		{
			if (typeid(*i.first) == t)
			{
				vector.push_back(std::make_pair<T, Gdiplus::Point>(T(i.first), Gdiplus::Point(i.second)));
			}
		}
		return vector;
	}
private:
	bool DoAdd(Gdiplus::Point location, T occupier, bool* occupyMask, int width, int height)
	{
		if (occupier == nullptr || Contains(occupier))
		{
			return false;
		}
		std::vector<Gdiplus::Point> array = GetOccupyPoints(location, occupyMask, width, height);
		if (from(array).any([&](Gdiplus::Point p) {return !Contains(p) || Get(p) == nullptr; }))
		{
			return false;
		}
		occupiers[occupier] = location;
		std::vector<Gdiplus::Point> array2 = array;
		for (unsigned int i = 0; i < array2.size(); i++)
		{
			Gdiplus::Point point = array2[i];
			occupierCells[(point.Y * metrics->Width) + point.X] = occupier;
		}
		return true;
	}
	bool DoRemove(T occupier)
	{
		Gdiplus::Point value;
		if (occupier == nullptr || !TryGetValue(occupiers, occupier, value))
		{
			return false;
		}
		occupiers.Remove(occupier);
		for (int i = value.Y; i < metrics->Length; i++)
		{
			for (int j = value.X; j < metrics->Width; j++)
			{
				if (occupierCells[(i * metrics->Width) + j] == occupier)
				{
					occupierCells[(i * metrics->Width) + j] = nullptr;
				}
			}
		}
		return true;
	}
	static std::vector<Gdiplus::Point> GetOccupyPoints(Gdiplus::Point location, bool *occupyMask, int width, int height)
	{
		std::vector<Gdiplus::Point> vector;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (occupyMask[(y * width) + x])
				{
					vector.push_back(location + Gdiplus::Size(x, y));
				}
			}
		}
		return vector;
	}
	static std::vector<Gdiplus::Point> GetOccupyPoints(Gdiplus::Point location, T occupier)
	{
		return GetOccupyPoints(location, occupier.OccupyMask, occupier.Width, occupier.Height);
	}
};
class Template
{
public:
	const TemplateType* type;
	int icon;
	bool* OccupyMask;
	int Width;
	int Height;
	Template() : type(nullptr), icon(0), Width(1), Height(1)
	{
		static bool mask[1] = { true };
		OccupyMask = mask;
	}
	Template& operator=(Template&) = default;
};
class Overlay
{
public:
	const OverlayType* type;
	int icon;
	bool* OccupyMask;
	int Width;
	int Height;
	Overlay() : type(nullptr), icon(0), OccupyMask(nullptr), Width(0), Height(0)
	{
	}
	Overlay& operator=(Overlay&) = default;
};
class Smudge
{
public:
	const SmudgeType* type;
	int icon;
	int data;
	Smudge() : type(nullptr), icon(0), data(0)
	{
	}
	Smudge& operator=(Smudge&) = default;
};
class Techno
{
public:
	bool* OccupyMask;
	int Width;
	int Height;
	Techno() : OccupyMask(nullptr), Width(0), Height(0)
	{
	}
	virtual ~Techno() {}
};
class Terrain : public Techno
{
public:
	const TerrainType* type;
	int icon;
	const char *trigger;
	Terrain() : type(nullptr), icon(0), trigger(nullptr)
	{
	}
	Terrain& operator=(Terrain&) = default;
};
class Map
{
public:
	std::string theater;
	int width;
	int height;
	int size;
	int theaterid;
	CellMetrics* metrics;
	CellGrid<Template>* templates;
	CellGrid<Overlay>* overlays;
	CellGrid<Smudge>* smudges;
	OccupierSet<Techno *> *technos;
	bool isra;
	int tilesize;
	Map();
	~Map();
	void Load(const char* path);
	void Render(Gdiplus::Graphics *graphics);
};