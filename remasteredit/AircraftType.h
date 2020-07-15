#pragma once
class HouseType;
struct DirectionType;
enum AircraftTypeTD {
	AIRCRAFTTD_TRANSPORT,
	AIRCRAFTTD_A10,
	AIRCRAFTTD_HELICOPTER,
	AIRCRAFTTD_CARGO,
	AIRCRAFTTD_ORCA,
	AIRCRAFTTD_COUNT,
	AIRCRAFTTD_NONE = -1,
	AIRCRAFTTD_FIRST = 0
}; 
enum AircraftTypeRA {
	AIRCRAFTRA_TRANSPORT,
	AIRCRAFTRA_BADGER,
	AIRCRAFTRA_U2,
	AIRCRAFTRA_MIG,
	AIRCRAFTRA_YAK,
	AIRCRAFTRA_LONGBOW,
	AIRCRAFTRA_HIND,
	AIRCRAFTRA_COUNT,
	AIRCRAFTRA_NONE = -1,
	AIRCRAFTRA_FIRST = 0
};
class AircraftType
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
	int RotorCount;
	AircraftType(char id, const char* name, const char* textid, const char* ownerhouse, int rotorcount, bool isra) : ID(id), Name(name), TextId(textid), Width(1), Height(1), OwnerHouse(ownerhouse), OverlapBounds(-1, -1, 3, 3), Thumbnail(nullptr), RotorCount(rotorcount)
	{
		static bool mask[1] = { true };
		if (isra)
		{
			AircraftMapRA[Name] = this;
		}
		else
		{
			AircraftMapTD[Name] = this;
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
	static std::map<std::string, AircraftType*> AircraftMapTD;
	static std::map<std::string, AircraftType*> AircraftMapRA;
	void Init(bool isra, HouseType* house, DirectionType direction);
};
