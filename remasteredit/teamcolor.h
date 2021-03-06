#pragma once
namespace pugi
{
	class xml_node;
};
class MegFileManager;
struct Vector3
{
	float X;
	float Y;
	float Z;
	Vector3() : X(0), Y(0), Z(0)
	{
	}
	Vector3(float x, float y, float z) : X(x), Y(y), Z(z)
	{
	}
};
struct Vector2
{
	float X;
	float Y;
	Vector2() : X(0), Y(0)
	{
	}
	Vector2(float x, float y) : X(x), Y(y)
	{
	}
	float LengthSquared()
	{
		return X * X + Y * Y;
	}
};
class TeamColor
{
public:
	Gdiplus::Color LowerBounds;
	Gdiplus::Color UpperBounds;
	float Fudge;
	Vector3 HSVShift;
	Vector3 InputLevels;
	Vector2 OutputLevels;
	Vector3 OverallInputLevels;
	Vector2 OverallOutputLevels;
	std::string Name;
	void Load(pugi::xml_node& node);
};
class TeamColorManager
{
private:
	MegFileManager* m;
	std::map<std::string, TeamColor*> teamColors;
public:
	TeamColor* GetColor(const char* color);
	TeamColorManager(MegFileManager* manager) : m(manager)
	{
	}
	void Load(const char* xmlPath);
};
extern TeamColorManager* TheTeamColorManagerTD;
extern TeamColorManager* TheTeamColorManagerRA;
void InitTeamColors();
void ShutdownTeamColors();
