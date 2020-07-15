#pragma once
#include "cellmetrics.h"
#include "boolinq.h"
using namespace boolinq;
class TemplateType;
class OverlayType;
class SmudgeType;
class TerrainType;
class StructType;
class HouseType;
class Map;
class AircraftType;
class VesselType;
inline std::set<Gdiplus::Point> GetPoints(Gdiplus::Rect rectangle)
{
	std::set<Gdiplus::Point> p;
	for (int y = rectangle.GetTop(); y < rectangle.GetBottom(); y++)
	{
		for (int x = rectangle.GetLeft(); x < rectangle.GetRight(); x++)
		{
			p.insert(Gdiplus::Point(x, y));
		}
	}
	return p;
}
template <typename T> inline void UnionWith(std::set<T>& set1, std::set<T>& set2)
{
	for (auto p : set2)
	{
		if (!set1.count(p))
		{
			set1.insert(p);
		}
	}
}
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
	Map* map;
public:
	Gdiplus::Size size;
	T Get(int x, int y)
	{
		return cells[(y * metrics->Width) + x];
	}
	void Set(int x, int y, T value)
	{
		int cell = (y * metrics->Width) + x;
		if (!cells[cell] || cells[cell]->type != value->type || cells[cell]->icon != value->icon)
		{
			T oldValue = cells[cell];
			cells[cell] = value;
			if (OnCellChanged)
			{
				std::invoke(OnCellChanged, map, CellChangedEventArgs<T>(metrics, Gdiplus::Point(x, y), oldValue, value));
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
	void (Map::*OnCellChanged)(CellChangedEventArgs<T> args);
	void (Map::*OnCleared)();
	CellGrid(CellMetrics* m, Map *ma) : metrics(m), OnCellChanged(nullptr), OnCleared(nullptr), map(ma)
	{
		metrics = m;
		cells = new T[metrics->Length];
		memset(cells, 0, sizeof(T) * metrics->Length);
	}
	void Clear()
	{
		for (int i = 0; i < metrics->Length; i++)
		{
			delete cells[i];
		}
		delete[] cells;
		cells = new T[metrics->Length];
		memset(cells, 0, sizeof(T) * metrics->Length);
		if (OnCleared)
		{
			std::invoke(OnCleared, map);
		}
	}
	T Adjacent(Gdiplus::Point location, FacingType facing)
	{
		Gdiplus::Point adjacent;
		if (!metrics->Adjacent(location, facing, adjacent))
		{
			return nullptr;
		}
		return Get(adjacent);
	}
	T Adjacent(int cell, FacingType facing)
	{
		Gdiplus::Point location;
		if (!metrics->GetLocation(cell, location))
		{
			return nullptr;
		}
		Gdiplus::Point adjacent;
		if (!metrics->Adjacent(location, facing, adjacent))
		{
			return nullptr;
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
		for (int i = 0; i < metrics->Length; i++)
		{
			delete cells[i];
		}
		delete[] cells;
	}
	std::vector<std::pair<int, T>> IntersectsWith(std::set<int> celllist)
	{
		std::vector<std::pair<int, T>> v;
		for (auto cell : celllist)
		{
			if (metrics->Contains(cell))
			{
				T val = Get(cell);
				if (val->type)
				{
					v.push_back(std::make_pair(cell, val));
				}
			}
		}
		return v;
	}
	std::vector<std::pair<int, T>> IntersectsWith(std::set<Gdiplus::Point> locations)
	{
		std::vector<std::pair<int, T>> v;
		for (auto location : locations)
		{
			if (metrics->Contains(location))
			{
				T val = Get(location);
				if (val->type)
				{
					int cell;
					metrics->GetCell(location, cell);
					v.push_back(std::make_pair(cell, val));
				}
			}
		}
		return v;
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
		if (occupiers.count(occupier))
		{
			return occupiers[occupier];
		}
		return Gdiplus::Point(-1,-1);
	}
	void (Map::*OnOccupierAdded)(OccupierEventArgs<T> args);
	void (Map::*OnOccupierRemoved)(OccupierEventArgs<T> args);
	auto begin()
	{
		return occupiers.begin();
	}
	auto end()
	{
		return occupiers.end();
	}
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
	void ClearOccupiers()
	{
		occupiers.clear();
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
			std::invoke(OnOccupierRemoved, map, OccupierEventArgs<T>(metrics, point, occupier));
		}
		return true;
	}
	bool Remove(Gdiplus::Point location)
	{
		return Remove(Get(location));
	}
	template <class type> std::vector<std::pair<type *, Gdiplus::Point>> GetOccupiers()
	{
		std::vector<std::pair<type *, Gdiplus::Point>> vector;
		for (auto i : occupiers)
		{
			type *t = dynamic_cast<type*>(i.first);
			if (t)
			{
				vector.push_back(std::make_pair(static_cast<type *>(t), static_cast<Gdiplus::Point>(i.second)));
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
		occupiers.erase(occupier);
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
template <class T> class OverlapperSet
{
public:
	CellMetrics* m;
	std::map<T, Gdiplus::Rect> overlappers;
	bool GetRect(T overlapper, Gdiplus::Rect& out)
	{
		if (Contains(overlapper))
		{
			out = overlappers[overlapper];
			return true;
		}
		else
		{
			return false;
		}
	}
	OverlapperSet(CellMetrics* metrics) : m(metrics)
	{
	}
	bool Add(Gdiplus::Point location, T overlapper)
	{
		if (overlapper == nullptr || Contains(overlapper))
		{
			return false;
		}
		Gdiplus::Rect overlapBounds = overlapper->OverlapBounds;
		overlapBounds.Offset(location);
		overlappers[overlapper] = overlapBounds;
		return true;
	}
	bool Add(int x, int y, T occupier)
	{
		return Add(Gdiplus::Point(x, y), occupier);
	}
	bool Add(int cell, T overlapper)
	{
		Gdiplus::Point location;
		if (!m->GetLocation(cell, location))
		{
			return false;
		}
		return Add(location, overlapper);
	}
	void Clear()
	{
		overlappers.clear();
	}
	bool Contains(T occupier)
	{
		return overlappers.count(occupier);
	}
	bool Remove(T overlapper)
	{
		Gdiplus::Rect rectangle;
		if (overlapper == nullptr || !TryGetValue(overlappers, overlapper, rectangle))
		{
			return false;
		}
		overlappers.erase(overlapper);
		return true;
	}
	std::set<Gdiplus::Point> Overlaps(std::set<Gdiplus::Rect> rectangles)
	{
		std::set<Gdiplus::Rect> rectangleSet = rectangles;
		unsigned int count;
		do
		{
			count = rectangleSet.size();
			auto other = from(overlappers).where([rectangleSet](std::pair<T, Gdiplus::Rect> x) {return from(rectangleSet).any([x](Gdiplus::Rect y) {return !!x.second.IntersectsWith(y); }); }).select([](std::pair<T, Gdiplus::Rect> r) {return r.second; }).toStdSet();
			UnionWith(rectangleSet, other);
		} while (rectangleSet.size() != count);
		std::set<Gdiplus::Point> points = from(rectangleSet).selectMany([](Gdiplus::Rect x) {return from(GetPoints(x)); }).toStdSet();
		return points;
	}
	std::set<Gdiplus::Point> Overlaps(std::set<Gdiplus::Point> points)
	{
		auto s = from(points).select([](Gdiplus::Point p) {return Gdiplus::Rect(p, Gdiplus::Size(1, 1)); }).toStdSet();
		return Overlaps(s);
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
class Overlapper
{
public:
	Gdiplus::Rect OverlapBounds;
	virtual ~Overlapper() {}
};
class Template : public Occupier
{
public:
	const TemplateType* type;
	int icon;
	Template() : type(nullptr), icon(0)
	{
		static bool mask[1] = { true };
		OccupyMask = mask;
		Width = 1;
		Height = 1;
	}
};
class Overlay : public Occupier
{
public:
	const OverlayType* type;
	int icon;
	Overlay() : type(nullptr), icon(0)
	{
		static bool mask[1] = { true };
		OccupyMask = mask;
		Width = 1;
		Height = 1;
	}
};
class Smudge : public Overlapper
{
public:
	const SmudgeType* type;
	int icon;
	int data;
	Smudge() : type(nullptr), icon(0), data(0)
	{
	}
};
class Terrain : public Occupier, public Overlapper
{
public:
	const TerrainType* type;
	int icon;
	std::string trigger;
	Terrain() : type(nullptr), icon(0), trigger("None")
	{
	}
};
class Structure : public Occupier, public Overlapper
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
	std::set<int> BibCells;
	Structure() : type(nullptr), strength(0), basePriority(-1), isPrebuilt(true), sellable(false), rebuild(false), trigger("None"), house(nullptr)
	{
	}
};
class Unit : public Occupier, public Overlapper
{
};
class Aircraft : public Unit
{
public:
	const AircraftType* type;
	HouseType* house;
	int strength;
	DirectionType direction;
	std::string mission;
	Aircraft() : type(nullptr), strength(0), house(nullptr)
	{
	}
};
class Vessel : public Unit
{
public:
	const VesselType* type;
	HouseType* house;
	int strength;
	DirectionType direction;
	std::string mission;
	std::string trigger;
	Vessel() : type(nullptr), strength(0), house(nullptr), trigger("None")
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
	CellGrid<Template*>* templates;
	CellGrid<Overlay*>* overlays;
	CellGrid<Smudge*>* smudges;
	OccupierSet<Occupier*>* technos;
	OccupierSet<Occupier*>* buildings;
	OverlapperSet<Overlapper*> *overlappers;
	bool isra;
	int updatecount;
	bool updating;
	std::map<MapLayerFlag, std::set<Gdiplus::Point>> invalidatelayers;
	bool invalidateoverlappers;
	Map();
	~Map();
	void Load(const char* path);
	void BeginUpdate()
	{
		updatecount++;
	}
	void EndUpdate()
	{
		updatecount--;
		if (!updatecount)
		{
			Update();
		}
	}
	void Update();
	void UpdateResourceOverlays(std::set<Gdiplus::Point> locations);
	void UpdateWallOverlays(std::set<Gdiplus::Point> locations);
	void RemoveBib(Structure* building);
	void AddBib(Gdiplus::Point location, Structure* s);
	void SmudgeCellChanged(CellChangedEventArgs<Smudge*> e);
	void OverlayCellChanged(CellChangedEventArgs<Overlay*> e);
	void TechnosOccupierAdded(OccupierEventArgs<Occupier*> args);
	void TechnosOccupierRemoved(OccupierEventArgs<Occupier*> args);
	void BuildingsOccupierAdded(OccupierEventArgs<Occupier*> args);
	void BuildingsOccupierRemoved(OccupierEventArgs<Occupier*> args);
	void Init();
	void Free();
};