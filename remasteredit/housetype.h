#pragma once
#include <string>
#include <map>
typedef enum HousesTypeTD : char {
	HOUSETD_NONE = -1,
	HOUSETD_GOOD,					// Global Defense Initiative
	HOUSETD_BAD,					// Brotherhood of Nod
	HOUSETD_NEUTRAL,				// Civilians
	HOUSETD_JP,					// Disaster Containment Team
	HOUSETD_MULTI1,				// Multi-Player house #1
	HOUSETD_MULTI2,				// Multi-Player house #2
	HOUSETD_MULTI3,				// Multi-Player house #3
	HOUSETD_MULTI4,				// Multi-Player house #4
	HOUSETD_MULTI5,				// Multi-Player house #5
	HOUSETD_MULTI6,				// Multi-Player house #6
	HOUSETD_COUNT,
	HOUSETD_FIRST = HOUSETD_GOOD
} HousesTypeTD;
typedef enum HousesTypeRA : char {
	HOUSERA_NONE = -1,
	HOUSERA_SPAIN,				//	Gold (unremapped)
	HOUSERA_GREECE,				// LtBlue
	HOUSERA_USSR,					// Red
	HOUSERA_ENGLAND,				// Green
	HOUSERA_UKRAINE,				// Orange
	HOUSERA_GERMANY,				// Grey
	HOUSERA_FRANCE,				// Blue
	HOUSERA_TURKEY,				// Brown
	HOUSERA_GOOD,					// Global Defense Initiative
	HOUSERA_BAD,					// Brotherhood of Nod
	HOUSERA_NEUTRAL,				// Civilians
	HOUSERA_JP,					// Disaster Containment Team
	HOUSERA_MULTI1,				// Multi-Player house #1
	HOUSERA_MULTI2,				// Multi-Player house #2
	HOUSERA_MULTI3,				// Multi-Player house #3
	HOUSERA_MULTI4,				// Multi-Player house #4
	HOUSERA_MULTI5,				// Multi-Player house #5
	HOUSERA_MULTI6,				// Multi-Player house #6
	HOUSERA_MULTI7,				// Multi-Player house #7
	HOUSERA_MULTI8,				// Multi-Player house #8
	HOUSERA_COUNT,
	HOUSERA_FIRST = 0
} HousesTypeRA;
class HouseType
{
public:
	int ID;
	std::string Name;
	std::string UnitColor;
	std::string StructColor;
	std::map<std::string, std::string> OverrideColors;
	HouseType(int id, const char* name, const char *unitcolor, const char *structcolor, int overridecount, const char **overridecolors, const char **overridenames, bool isra) : ID(id), Name(name), UnitColor(unitcolor), StructColor(structcolor)
	{
		for (int i = 0; i < overridecount; i++)
		{
			OverrideColors[overridenames[i]] = overridecolors[i];
		}
		if (isra)
		{
			HouseMapRA[name] = this;
		}
		else
		{
			HouseMapTD[name] = this;
		}
	}
	static std::map<std::string, HouseType*> HouseMapTD;
	static std::map<std::string, HouseType*> HouseMapRA;

};