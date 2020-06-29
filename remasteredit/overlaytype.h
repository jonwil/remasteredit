#pragma once
#include "theatertype.h"
#include <string>
#include <map>
#include <vector>
typedef enum OverlayTypeFlag
{
	OVERLAYTYPE_NONE,
	OVERLAYTYPE_TIBERIUMORGOLD,
	OVERLAYTYPE_GEMS,
	OVERLAYTYPE_WALL,
	OVERLAYTYPE_CRATE,
	OVERLAYTYPE_FLAG
} OverlayTypeFlag;
typedef enum OverlayTypeTD : char {
	OVERLAYTD_NONE = -1,
	OVERLAYTD_CONCRETE,
	OVERLAYTD_SANDBAG_WALL,
	OVERLAYTD_CYCLONE_WALL,
	OVERLAYTD_BRICK_WALL,
	OVERLAYTD_BARBWIRE_WALL,
	OVERLAYTD_WOOD_WALL,
	OVERLAYTD_TIBERIUM1,
	OVERLAYTD_TIBERIUM2,
	OVERLAYTD_TIBERIUM3,
	OVERLAYTD_TIBERIUM4,
	OVERLAYTD_TIBERIUM5,
	OVERLAYTD_TIBERIUM6,
	OVERLAYTD_TIBERIUM7,
	OVERLAYTD_TIBERIUM8,
	OVERLAYTD_TIBERIUM9,
	OVERLAYTD_TIBERIUM10,
	OVERLAYTD_TIBERIUM11,
	OVERLAYTD_TIBERIUM12,
	OVERLAYTD_ROAD,
	OVERLAYTD_SQUISH,
	OVERLAYTD_V12,
	OVERLAYTD_V13,
	OVERLAYTD_V14,
	OVERLAYTD_V15,
	OVERLAYTD_V16,
	OVERLAYTD_V17,
	OVERLAYTD_V18,
	OVERLAYTD_FLAG_SPOT,
	OVERLAYTD_WOOD_CRATE,
	OVERLAYTD_STEEL_CRATE,
	OVERLAYTD_COUNT,
	OVERLAYTD_FIRST = 0
} OverlayTypeTD;
typedef enum OverlayTypeRA : char {
	OVERLAYRA_NONE = -1,
	OVERLAYRA_SANDBAG_WALL,
	OVERLAYRA_CYCLONE_WALL,
	OVERLAYRA_BRICK_WALL,
	OVERLAYRA_BARBWIRE_WALL,
	OVERLAYRA_WOOD_WALL,
	OVERLAYRA_GOLD1,
	OVERLAYRA_GOLD2,
	OVERLAYRA_GOLD3,
	OVERLAYRA_GOLD4,
	OVERLAYRA_GEMS1,
	OVERLAYRA_GEMS2,
	OVERLAYRA_GEMS3,
	OVERLAYRA_GEMS4,
	OVERLAYRA_V12,
	OVERLAYRA_V13,
	OVERLAYRA_V14,
	OVERLAYRA_V15,
	OVERLAYRA_V16,
	OVERLAYRA_V17,
	OVERLAYRA_V18,
	OVERLAYRA_FLAG_SPOT,
	OVERLAYRA_WOOD_CRATE,
	OVERLAYRA_STEEL_CRATE,
	OVERLAYRA_FENCE,
	OVERLAYRA_WATER_CRATE,
	OVERLAYRA_COUNT,
	OVERLAYRA_FIRST = 0
} OverlayTypeRA;
class OverlayType
{
public:
	char ID;
	std::string Name;
	std::string TextId;
	unsigned char Theater;
	OverlayTypeFlag Flag;
	bool* OccupyMask;
	int Width;
	int Height;
	OverlayType(char id, const char* name, const char *textid, unsigned char theater, OverlayTypeFlag flag, bool isra) : ID(id), Name(name), TextId(textid), Theater(theater), Flag(flag), Width(1), Height(1)
	{
		static bool mask[1] = { true };
		if (isra)
		{
			OverlayMapRA[name] = this;
		}
		else
		{
			OverlayMapTD[name] = this;
		}
		if (flag == OVERLAYTYPE_TIBERIUMORGOLD && isra)
		{
			Gold.push_back(this);
		}
		if (flag == OVERLAYTYPE_TIBERIUMORGOLD && !isra)
		{
			Tiberium.push_back(this);
		}
		if (flag == OVERLAYTYPE_GEMS)
		{
			Gems.push_back(this);
		}
		OccupyMask = mask;
	}
	static OverlayType const* const PointersRA[OVERLAYRA_COUNT];
	static OverlayType const* const PointersTD[OVERLAYTD_COUNT];
	static std::map<std::string, OverlayType*> OverlayMapTD;
	static std::map<std::string, OverlayType*> OverlayMapRA;
	static std::vector<OverlayType*> Tiberium;
	static std::vector<OverlayType*> Gold;
	static std::vector<OverlayType*> Gems;
};
