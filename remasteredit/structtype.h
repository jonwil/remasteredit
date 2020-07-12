#pragma once
class HouseType;
struct DirectionType;
typedef enum StructTypeTD : char {
	STRUCTTD_NONE = -1,
	STRUCTTD_WEAP,
	STRUCTTD_GTOWER,
	STRUCTTD_ATOWER,
	STRUCTTD_OBELISK,
	STRUCTTD_RADAR,
	STRUCTTD_TURRET,
	STRUCTTD_CONST,
	STRUCTTD_REFINERY,
	STRUCTTD_STORAGE,
	STRUCTTD_HELIPAD,
	STRUCTTD_SAM,
	STRUCTTD_AIRSTRIP,
	STRUCTTD_POWER,
	STRUCTTD_ADVANCED_POWER,
	STRUCTTD_HOSPITAL,
	STRUCTTD_BARRACKS,
	STRUCTTD_TANKER,
	STRUCTTD_REPAIR,
	STRUCTTD_BIO_LAB,
	STRUCTTD_HAND,
	STRUCTTD_TEMPLE,
	STRUCTTD_EYE,
	STRUCTTD_MISSION,
	STRUCTTD_V01,
	STRUCTTD_V02,
	STRUCTTD_V03,
	STRUCTTD_V04,
	STRUCTTD_V05,
	STRUCTTD_V06,
	STRUCTTD_V07,
	STRUCTTD_V08,
	STRUCTTD_V09,
	STRUCTTD_V10,
	STRUCTTD_V11,
	STRUCTTD_V12,
	STRUCTTD_V13,
	STRUCTTD_V14,
	STRUCTTD_V15,
	STRUCTTD_V16,
	STRUCTTD_V17,
	STRUCTTD_V18,
	STRUCTTD_PUMP,
	STRUCTTD_V20,
	STRUCTTD_V21,
	STRUCTTD_V22,
	STRUCTTD_V23,
	STRUCTTD_V24,
	STRUCTTD_V25,
	STRUCTTD_V26,
	STRUCTTD_V27,
	STRUCTTD_V28,
	STRUCTTD_V29,
	STRUCTTD_V30,
	STRUCTTD_V31,
	STRUCTTD_V32,
	STRUCTTD_V33,
	STRUCTTD_V34,
	STRUCTTD_V35,
	STRUCTTD_V36,
	STRUCTTD_V37,
	STRUCTTD_SANDBAG_WALL,
	STRUCTTD_CYCLONE_WALL,
	STRUCTTD_BRICK_WALL,
	STRUCTTD_BARBWIRE_WALL,
	STRUCTTD_WOOD_WALL,
	STRUCTTD_COUNT,
	STRUCTTD_FIRST = 0
} StructTypeTD;
typedef enum StructTypeRA : char {
	STRUCTRA_NONE = -1,
	STRUCTRA_ADVANCED_TECH,
	STRUCTRA_IRON_CURTAIN,
	STRUCTRA_WEAP,
	STRUCTRA_CHRONOSPHERE,
	STRUCTRA_PILLBOX,
	STRUCTRA_CAMOPILLBOX,
	STRUCTRA_RADAR,
	STRUCTRA_GAP,
	STRUCTRA_TURRET,
	STRUCTRA_AAGUN,
	STRUCTRA_FLAME_TURRET,
	STRUCTRA_CONST,
	STRUCTRA_REFINERY,
	STRUCTRA_STORAGE,
	STRUCTRA_HELIPAD,
	STRUCTRA_SAM,
	STRUCTRA_AIRSTRIP,
	STRUCTRA_POWER,
	STRUCTRA_ADVANCED_POWER,
	STRUCTRA_SOVIET_TECH,
	STRUCTRA_HOSPITAL,
	STRUCTRA_BARRACKS,
	STRUCTRA_TENT,
	STRUCTRA_KENNEL,
	STRUCTRA_REPAIR,
	STRUCTRA_BIO_LAB,
	STRUCTRA_MISSION,
	STRUCTRA_SHIP_YARD,
	STRUCTRA_SUB_PEN,
	STRUCTRA_MSLO,
	STRUCTRA_FORWARD_COM,
	STRUCTRA_TESLA,
	STRUCTRA_FAKEWEAP,
	STRUCTRA_FAKECONST,
	STRUCTRA_FAKE_YARD,
	STRUCTRA_FAKE_PEN,
	STRUCTRA_FAKE_RADAR,
	STRUCTRA_SANDBAG_WALL,
	STRUCTRA_CYCLONE_WALL,
	STRUCTRA_BRICK_WALL,
	STRUCTRA_BARBWIRE_WALL,
	STRUCTRA_WOOD_WALL,
	STRUCTRA_FENCE,
	STRUCTRA_AVMINE,
	STRUCTRA_APMINE,
	STRUCTRA_V01,
	STRUCTRA_V02,
	STRUCTRA_V03,
	STRUCTRA_V04,
	STRUCTRA_V05,
	STRUCTRA_V06,
	STRUCTRA_V07,
	STRUCTRA_V08,
	STRUCTRA_V09,
	STRUCTRA_V10,
	STRUCTRA_V11,
	STRUCTRA_V12,
	STRUCTRA_V13,
	STRUCTRA_V14,
	STRUCTRA_V15,
	STRUCTRA_V16,
	STRUCTRA_V17,
	STRUCTRA_V18,
	STRUCTRA_PUMP,
	STRUCTRA_V20,
	STRUCTRA_V21,
	STRUCTRA_V22,
	STRUCTRA_V23,
	STRUCTRA_V24,
	STRUCTRA_V25,
	STRUCTRA_V26,
	STRUCTRA_V27,
	STRUCTRA_V28,
	STRUCTRA_V29,
	STRUCTRA_V30,
	STRUCTRA_V31,
	STRUCTRA_V32,
	STRUCTRA_V33,
	STRUCTRA_V34,
	STRUCTRA_V35,
	STRUCTRA_V36,
	STRUCTRA_V37,
	STRUCTRA_BARREL,
	STRUCTRA_BARREL3,
	STRUCTRA_QUEEN,
	STRUCTRA_LARVA1,
	STRUCTRA_LARVA2,
	STRUCTRA_COUNT,
	STRUCTRA_FIRST = 0
} StructTypeRA;
class StructType
{
public:
	char ID;
	std::string Name;
	std::string TextId;
	std::string TileName;
	Gdiplus::Rect OverlapBounds;
	int OccupyWidth;
	int OccupyHeight;
	bool* OccupyMask;
	int Width;
	int Height;
	bool* BaseOccupyMask;
	Gdiplus::Size Size;
	bool HasBib;
	std::string OwnerHouse;
	unsigned char Theater;
	bool IsFake;
	bool HasTurret;
	std::string FactoryOverlay;
	Gdiplus::Bitmap *Thumbnail;
	StructType(char id, const char* name, const char* textid, int height, int width, bool* baseoccupymask, int occupyheight, int occupywidth, bool* occupymask, bool hasbib, const char* ownerhouse, unsigned char theater, bool isfake, bool hasturret, const char* factoryoverlay, bool isra) : ID(id), Name(name), TileName(name), TextId(textid), Width(width), Height(height), BaseOccupyMask(baseoccupymask), OccupyWidth(occupywidth), OccupyHeight(occupyheight), OccupyMask(occupymask), HasBib(hasbib), OwnerHouse(ownerhouse), Theater(theater), IsFake(isfake), HasTurret(hasturret), FactoryOverlay(factoryoverlay), Size(Width, Height), OverlapBounds(0, 0, OccupyWidth, OccupyHeight)
	{
		if (isfake)
		{
			Name[Name.length() - 1] = 'f';
		}
		if (isra)
		{
			StructMapRA[Name] = this;
		}
		else
		{
			StructMapTD[Name] = this;
		}
	}
	void Free()
	{
		if (Thumbnail)
		{
			delete[] Thumbnail;
			Thumbnail = nullptr;
		}
	}
	static std::map<std::string, StructType*> StructMapTD;
	static std::map<std::string, StructType*> StructMapRA;
	void Init(bool isra, HouseType* house, DirectionType direction);
};
