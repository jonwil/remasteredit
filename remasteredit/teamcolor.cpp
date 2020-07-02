#include "teamcolor.h"
#include "pugixml.hpp"
#include "megfilemanager.h"
#include "WWFILE.H"

void TeamColor::Load(pugi::xml_node& node)
{
	Name = node.attribute("Name").value();
	pugi::xml_node xmlNode = node.select_node("LowerBounds").node();
	if (xmlNode != nullptr)
	{
		LowerBounds = Gdiplus::Color::MakeARGB(0, (BYTE)(atof(xmlNode.select_node("R").node().child_value()) * 255.0f), (BYTE)(atof(xmlNode.select_node("G").node().child_value()) * 255.0f), (BYTE)(atof(xmlNode.select_node("B").node().child_value()) * 255.0f));
	}
	pugi::xml_node xmlNode2 = node.select_node("UpperBounds").node();
	if (xmlNode2 != nullptr)
	{
		UpperBounds = Gdiplus::Color::MakeARGB(0, (BYTE)(atof(xmlNode2.select_node("R").node().child_value()) * 255.0f), (BYTE)(atof(xmlNode2.select_node("G").node().child_value()) * 255.0f), (BYTE)(atof(xmlNode2.select_node("B").node().child_value()) * 255.0f));
	}
	pugi::xml_node xmlNode3 = node.select_node("Fudge").node();
	if (xmlNode3 != nullptr)
	{
		Fudge = (float)atof(xmlNode3.child_value());
	}
	pugi::xml_node xmlNode4 = node.select_node("HSVShift").node();
	if (xmlNode4 != nullptr)
	{
		HSVShift = Vector3((float)atof(xmlNode4.select_node("X").node().child_value()), (float)atof(xmlNode4.select_node("Y").node().child_value()), (float)atof(xmlNode4.select_node("Z").node().child_value()));
	}
	pugi::xml_node xmlNode5 = node.select_node("InputLevels").node();
	if (xmlNode5 != nullptr)
	{
		InputLevels = Vector3((float)atof(xmlNode5.select_node("X").node().child_value()), (float)atof(xmlNode5.select_node("Y").node().child_value()), (float)atof(xmlNode5.select_node("Z").node().child_value()));
	}
	pugi::xml_node xmlNode6 = node.select_node("OutputLevels").node();
	if (xmlNode6 != nullptr)
	{
		OutputLevels = Vector2((float)atof(xmlNode6.select_node("X").node().child_value()), (float)atof(xmlNode6.select_node("Y").node().child_value()));
	}
	pugi::xml_node xmlNode7 = node.select_node("OverallInputLevels").node();
	if (xmlNode7 != nullptr)
	{
		OverallInputLevels = Vector3((float)atof(xmlNode7.select_node("X").node().child_value()), (float)atof(xmlNode7.select_node("Y").node().child_value()), (float)atof(xmlNode7.select_node("Z").node().child_value()));
	}
	pugi::xml_node xmlNode8 = node.select_node("OverallOutputLevels").node();
	if (xmlNode6 != nullptr)
	{
		OverallOutputLevels = Vector2((float)atof(xmlNode8.select_node("X").node().child_value()), (float)atof(xmlNode8.select_node("Y").node().child_value()));
	}
}
TeamColor* TeamColorManager::GetColor(const char* color)
{
	return teamColors[color];
}

void TeamColorManager::Load(const char* xmlPath)
{
	pugi::xml_document doc;
	FileClass* f = m->Open(xmlPath);
	int size = f->Size();
	char* buffer = new char[size];
	f->Read(buffer, size);
	doc.load_buffer(buffer, size);
	for (pugi::xpath_node n : doc.select_nodes("/*/TeamColorTypeClass"))
	{
		pugi::xml_node item = n.node();
		TeamColor *teamColor = new TeamColor();
		teamColor->Load(item);
		teamColors[item.attribute("Name").value()] = teamColor;
	}
}
TeamColorManager* TheTeamColorManagerTD;
TeamColorManager* TheTeamColorManagerRA;
void InitTeamColors()
{
	TheTeamColorManagerTD = new TeamColorManager(TheMegFileManager);
	TheTeamColorManagerRA = new TeamColorManager(TheMegFileManager);
	TheTeamColorManagerTD->Load("DATA\\XML\\CNCTDTEAMCOLORS.XML");
	TheTeamColorManagerRA->Load("DATA\\XML\\CNCRATEAMCOLORS.XML");
}

void ShutdownTeamColors()
{
	delete TheTeamColorManagerTD;
	delete TheTeamColorManagerRA;
}
