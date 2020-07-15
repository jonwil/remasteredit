#pragma once
class HouseType;
struct DirectionType;
enum VesselTypeRA {
	VESSELRA_NONE = -1,
	VESSELRA_SS,
	VESSELRA_DD,
	VESSELRA_CA,
	VESSELRA_TRANSPORT,
	VESSELRA_PT,
	VESSELRA_MISSILESUB,
	VESSELRA_CARRIER,
	VESSELRA_COUNT,
	VESSELRA_FIRST = 0
};
class VesselType
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
	VesselType(char id, const char* name, const char* textid, const char* ownerhouse, bool hasturret, bool isra) : ID(id), Name(name), TextId(textid), Width(1), Height(1), OwnerHouse(ownerhouse), OverlapBounds(-1, -1, 3, 3), Thumbnail(nullptr), HasTurret(hasturret)
	{
		static bool mask[1] = { true };
		if (isra)
		{
			VesselMapRA[Name] = this;
		}
		else
		{
			VesselMapTD[Name] = this;
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
	static std::map<std::string, VesselType*> VesselMapTD;
	static std::map<std::string, VesselType*> VesselMapRA;
	void Init(bool isra, HouseType* house, DirectionType direction);
};
