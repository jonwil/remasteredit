#pragma once
#include <string>
enum FacingType
{
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
namespace Gdiplus
{
	class Graphics;
}
class TemplateType;
class OverlayType;
class SmudgeType;
class TerrainType;
class Template
{
public:
	const TemplateType* type;
	int icon;
};
class Overlay
{
public:
	const OverlayType* type;
	int icon;
};
class Smudge
{
public:
	const SmudgeType* type;
	int icon;
	int data;
};
class Terrain
{
public:
	const TerrainType* type;
	int icon;
	const char *Trigger;
};
class Map
{
public:
	std::string theater;
	int width;
	int height;
	int size;
	int theaterid;
	Template* templates;
	Overlay* overlays;
	Smudge* smudges;
	Terrain* terrains;
	bool isra;
	int tilesize;
	Map();
	~Map();
	void Load(const char* path);
	void Render(Gdiplus::Graphics *graphics);
	bool GetAdjacentPoint(int x, int y, FacingType facing, int& outx, int& outy);
	Overlay *GetAdjacentOverlay(int x, int y, FacingType facing);
};