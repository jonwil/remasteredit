#pragma once
#include <string>
#include <map>
typedef enum TerrainTypeTD : char {
	TERRAINTD_NONE = -1,
	TERRAINTD_TREE1,
	TERRAINTD_TREE2,
	TERRAINTD_TREE3,
	TERRAINTD_TREE4,
	TERRAINTD_TREE5,
	TERRAINTD_TREE6,
	TERRAINTD_TREE7,
	TERRAINTD_TREE8,
	TERRAINTD_TREE9,
	TERRAINTD_TREE10,
	TERRAINTD_TREE11,
	TERRAINTD_TREE12,
	TERRAINTD_TREE13,
	TERRAINTD_TREE14,
	TERRAINTD_TREE15,
	TERRAINTD_TREE16,
	TERRAINTD_TREE17,
	TERRAINTD_TREE18,
	TERRAINTD_BLOSSOMTREE1,
	TERRAINTD_BLOSSOMTREE2,
	TERRAINTD_CLUMP1,
	TERRAINTD_CLUMP2,
	TERRAINTD_CLUMP3,
	TERRAINTD_CLUMP4,
	TERRAINTD_CLUMP5,
	TERRAINTD_ROCK1,
	TERRAINTD_ROCK2,
	TERRAINTD_ROCK3,
	TERRAINTD_ROCK4,
	TERRAINTD_ROCK5,
	TERRAINTD_ROCK6,
	TERRAINTD_ROCK7,
	TERRAINTD_COUNT,
	TERRAINTD_FIRST = 0
} TerrainTypeTD;
typedef enum TerrainTypeRA : char {
	TERRAINRA_NONE = -1,
	TERRAINRA_TREE1,
	TERRAINRA_TREE2,
	TERRAINRA_TREE3,
	TERRAINRA_TREE5,
	TERRAINRA_TREE6,
	TERRAINRA_TREE7,
	TERRAINRA_TREE8,
	TERRAINRA_TREE10,
	TERRAINRA_TREE11,
	TERRAINRA_TREE12,
	TERRAINRA_TREE13,
	TERRAINRA_TREE14,
	TERRAINRA_TREE15,
	TERRAINRA_TREE16,
	TERRAINRA_TREE17,
	TERRAINRA_CLUMP1,
	TERRAINRA_CLUMP2,
	TERRAINRA_CLUMP3,
	TERRAINRA_CLUMP4,
	TERRAINRA_CLUMP5,
	TERRAINRA_ICE01,
	TERRAINRA_ICE02,
	TERRAINRA_ICE03,
	TERRAINRA_ICE04,
	TERRAINRA_ICE05,
	TERRAINRA_BOXES01,
	TERRAINRA_BOXES02,
	TERRAINRA_BOXES03,
	TERRAINRA_BOXES04,
	TERRAINRA_BOXES05,
	TERRAINRA_BOXES06,
	TERRAINRA_BOXES07,
	TERRAINRA_BOXES08,
	TERRAINRA_BOXES09,
	TERRAINRA_MINE,
	TERRAINRA_COUNT,
	TERRAINRA_FIRST = 0
} TerrainTypeRA;
class TerrainType
{
public:
	char ID;
	std::string Name;
	unsigned char Theater;
	bool IsTransformable;
	bool IsMine;
	int Width;
	int Height;
	bool* OccupyMask;
	TerrainType(char id, const char* name, unsigned char theater, bool istransformable, bool ismine, int width, int height, bool* occupymask, bool isra) : ID(id), Name(name), Theater(theater), IsTransformable(istransformable), IsMine(ismine), Width(width), Height(height), OccupyMask(occupymask)
	{
		if (isra)
		{
			TerrainMapRA[name] = this;
		}
		else
		{
			TerrainMapTD[name] = this;
		}
	}
	static TerrainType const* const PointersRA[TERRAINRA_COUNT];
	static TerrainType const* const PointersTD[TERRAINTD_COUNT];
	static std::map<std::string, TerrainType*> TerrainMapTD;
	static std::map<std::string, TerrainType*> TerrainMapRA;
};
