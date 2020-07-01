#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlwapi.h>
#pragma warning(disable: 4458)
#include <gdiplus.h>
#pragma warning(default: 4458)
#include <string>
#include <map>
typedef enum SmudgeTypeFlag : char {
	SMUDGETYPE_NONE = 0x0,
	SMUDGETYPE_BIB = 0x1,
	SMUDGETYPE_BIB1 = 0x3,
	SMUDGETYPE_BIB2 = 0x5,
	SMUDGETYPE_BIB3 = 0x9
} SmudgeTypeFlag;
typedef enum SmudgeTypeTD : char {
	SMUDGETD_NONE = -1,
	SMUDGETD_CRATER1,
	SMUDGETD_CRATER2,
	SMUDGETD_CRATER3,
	SMUDGETD_CRATER4,
	SMUDGETD_CRATER5,
	SMUDGETD_CRATER6,
	SMUDGETD_SCORCH1,
	SMUDGETD_SCORCH2,
	SMUDGETD_SCORCH3,
	SMUDGETD_SCORCH4,
	SMUDGETD_SCORCH5,
	SMUDGETD_SCORCH6,
	SMUDGETD_BIB1,
	SMUDGETD_BIB2,
	SMUDGETD_BIB3,
	SMUDGETD_COUNT,
	SMUDGETD_FIRST = 0
} SmudgeTypeTD;
typedef enum SmudgeTypeRA : char {
	SMUDGERA_NONE = -1,
	SMUDGERA_CRATER1,
	SMUDGERA_CRATER2,
	SMUDGERA_CRATER3,
	SMUDGERA_CRATER4,
	SMUDGERA_CRATER5,
	SMUDGERA_CRATER6,
	SMUDGERA_SCORCH1,
	SMUDGERA_SCORCH2,
	SMUDGERA_SCORCH3,
	SMUDGERA_SCORCH4,
	SMUDGERA_SCORCH5,
	SMUDGERA_SCORCH6,
	SMUDGERA_BIB1,
	SMUDGERA_BIB2,
	SMUDGERA_BIB3,
	SMUDGERA_COUNT,
	SMUDGERA_FIRST = 0
} SmudgeTypeRA;
class SmudgeType
{
public:
	char ID;
	std::string Name;
	int Width;
	int Height;
	SmudgeTypeFlag Flag;
	Gdiplus::Size Size;
	SmudgeType(char id, const char* name, int width, int height, SmudgeTypeFlag flag, bool isra = false) : ID(id), Name(name), Width(width), Height(height), Flag(flag), Size(Width, Height)
	{
		if (isra)
		{
			SmudgeMapRA[name] = this;
		}
		else
		{
			SmudgeMapTD[name] = this;
		}
	}
	static SmudgeType const* const PointersRA[SMUDGERA_COUNT];
	static SmudgeType const* const PointersTD[SMUDGETD_COUNT];
	static std::map<std::string, SmudgeType*> SmudgeMapTD;
	static std::map<std::string, SmudgeType*> SmudgeMapRA;
};
