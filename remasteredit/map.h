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
class StructType;
class HouseType;
class Map;
struct DirectionType
{
	unsigned char ID;
	std::string Name;
	FacingType Facing;
	static DirectionType Types[16];
};
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
	CellGrid(CellMetrics* m) : metrics(m), OnCellChanged(nullptr), OnCleared(nullptr)
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
	Map* map;
public:
	T Get(Gdiplus::Point location)
	{
		return Get(location.X, location.Y);
	}
	T Get(int x, int y)
	{
		if (!Contains(x, y))
		{
			return nullptr;
		}
		return occupierCells[(y * metrics->Width) + x];
	}
	T Get(int cell)
	{
		Gdiplus::Point location;
		if (!metrics->GetLocation(cell, location))
		{
			return nullptr;
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
	void (Map::*OnOccupierAdded)(OccupierEventArgs<T> args);
	void (Map::*OnOccupierRemoved)(OccupierEventArgs<T> args);
	OccupierSet(CellMetrics* m) : metrics(m), OnOccupierAdded(nullptr), OnOccupierRemoved(nullptr)
	{
		occupierCells = new T[metrics->Length];
		memset(occupierCells, 0, sizeof(T) * metrics->Length);
	}
	~OccupierSet()
	{
		for (auto i : occupiers)
		{
			delete i.first;
		}
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
			std::invoke(OnOccupierAdded, map, OccupierEventArgs<T>(metrics, location, occupier));
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
		for (auto i : occupiers)
		{
			delete i.first;
		}
		occupiers.clear();
		delete[] occupierCells;
		occupierCells = new T[metrics->Length];
		memset(occupierCells, 0, sizeof(T) * metrics->Length);
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
			std::invoke(OnOccupierRemoved, map, OccupierEventArgs<T>(metrics, point.Value, occupier));
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
		if (from(array).any([&](Gdiplus::Point p) {return !Contains(p) || Get(p) != nullptr; }))
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
		delete occupier;
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
	friend class Map;
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
};
class Occupier
{
public:
	bool* OccupyMask;
	int Width;
	int Height;
	Occupier() : OccupyMask(nullptr), Width(0), Height(0)
	{
	}
	virtual ~Occupier() {}
};
class Terrain : public Occupier
{
public:
	const TerrainType* type;
	int icon;
	std::string trigger;
	Terrain() : type(nullptr), icon(0)
	{
	}
};
class Structure : public Occupier
{
public:
	const StructType* type;
	HouseType* house;
	int strength;
	DirectionType direction;
	std::string trigger;
	int basePriority;
	bool isPrebuilt;
	bool sellable;
	bool rebuild;
	std::vector<int> BibCells;
	Structure() : type(nullptr), strength(0), basePriority(-1), isPrebuilt(true), sellable(false), rebuild(false)
	{
	}
};
class Map
{
public:
	std::string theater;
	int width;
	int height;
	int size;
	int theaterid;
	HouseType* player;
	HouseType* baseplayer;
	CellMetrics* metrics;
	CellGrid<Template>* templates;
	CellGrid<Overlay>* overlays;
	CellGrid<Smudge>* smudges;
	OccupierSet<Occupier*>* technos;
	OccupierSet<Occupier*>* buildings;
	bool isra;
	int tilesize;
	Map();
	~Map();
	void Load(const char* path);
	void BuildingsOccupierAddedRA(OccupierEventArgs<Occupier*> args);
	void AddBibRA(Gdiplus::Point location, Structure* s);
	void BuildingsOccupierAddedTD(OccupierEventArgs<Occupier*> args);
	void AddBibTD(Gdiplus::Point location, Structure* s);
	void Render(Gdiplus::Graphics *graphics);
};