#pragma once
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
	Gdiplus::Rect OverlapBounds;
	Gdiplus::Size Size;
	SmudgeTypeFlag Flag;
	Gdiplus::Size RenderSize;
	Gdiplus::Bitmap* Thumbnail;
	SmudgeType(char id, const char* name, Gdiplus::Size size, SmudgeTypeFlag flag, bool isra = false) : ID(id), Name(name), Flag(flag), Size(size), Thumbnail(nullptr)
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
	void Free()
	{
		if (Thumbnail)
		{
			delete Thumbnail;
			Thumbnail = nullptr;
		}
	}
	static std::map<std::string, SmudgeType*> SmudgeMapTD;
	static std::map<std::string, SmudgeType*> SmudgeMapRA;
	void Init();
};
