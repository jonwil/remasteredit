#pragma once
class HouseType;
struct DirectionType;
enum UnitTypeTD {
	UNITTD_NONE = -1,
	UNITTD_HTANK,
	UNITTD_MTANK,
	UNITTD_LTANK,
	UNITTD_STANK,
	UNITTD_FTANK,
	UNITTD_VICE,
	UNITTD_APC,
	UNITTD_MLRS,
	UNITTD_JEEP,
	UNITTD_BUGGY,
	UNITTD_HARVESTER,
	UNITTD_ARTY,
	UNITTD_MSAM,
	UNITTD_HOVER,
	UNITTD_MHQ,
	UNITTD_GUNBOAT,
	UNITTD_MCV,
	UNITTD_BIKE,
	UNITTD_TRIC,
	UNITTD_TREX,
	UNITTD_RAPT,
	UNITTD_STEG,
	UNITTD_COUNT,
	UNITTD_FIRST = 0
};
enum UnitTypeRA {
	UNITRA_NONE = -1,
	UNITRA_HTANK,
	UNITRA_MTANK,
	UNITRA_MTANK2,
	UNITRA_LTANK,
	UNITRA_APC,
	UNITRA_MINELAYER,
	UNITRA_JEEP,
	UNITRA_HARVESTER,
	UNITRA_ARTY,
	UNITRA_MRJ,
	UNITRA_MGG,
	UNITRA_MCV,
	UNITRA_V2_LAUNCHER,
	UNITRA_TRUCK,
	UNITRA_ANT1,
	UNITRA_ANT2,
	UNITRA_ANT3,
	UNITRA_CHRONOTANK,
	UNITRA_TESLATANK,
	UNITRA_MAD,
	UNITRA_DEMOTRUCK,
	UNITRA_PHASE,
	UNITRA_COUNT,
	UNITRA_FIRST = 0
};
class UnitType
{
public:
	char ID;
	std::string Name;
	std::string TextId;
	Gdiplus::Rect OverlapBounds;
	int Width;
	int Height;
	bool* OccupyMask;
	std::string OwnerHouse;
	Gdiplus::Size RenderSize;
	Gdiplus::Bitmap* Thumbnail;
	bool HasTurret;
	bool HasRadar;
	bool EightFrame;
	bool FourFrame;
	bool IsAnimating;
	UnitType(char id, const char* name, const char* textid, const char* ownerhouse, bool hasturret, bool hasradar, bool eightframe, bool fourframe, bool isanimating, bool isra) : ID(id), Name(name), TextId(textid), Width(1), Height(1), OwnerHouse(ownerhouse), OverlapBounds(-1, -1, 3, 3), Thumbnail(nullptr), HasTurret(hasturret), HasRadar(hasradar), EightFrame(eightframe), FourFrame(fourframe), IsAnimating(isanimating)
	{
		static bool mask[1] = { true };
		if (isra)
		{
			UnitMapRA[Name] = this;
		}
		else
		{
			UnitMapTD[Name] = this;
		}
		OccupyMask = mask;
	}
	void Free()
	{
		if (Thumbnail)
		{
			delete[] Thumbnail;
			Thumbnail = nullptr;
		}
	}
	static std::map<std::string, UnitType*> UnitMapTD;
	static std::map<std::string, UnitType*> UnitMapRA;
	void Init(bool isra, HouseType* house, DirectionType direction);
};
