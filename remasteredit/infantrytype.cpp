#include "global.h"
#include "infantrytype.h"
#include "map.h"
#include "maprender.h"
#include "tileset.h"
std::map<std::string, InfantryType*> InfantryType::InfantryMapTD;
std::map<std::string, InfantryType*> InfantryType::InfantryMapRA;
static InfantryType TDE1(INFANTRYTD_E1, "e1", "TEXT_UNIT_TITLE_GDI_MINIGUNNER", "Goodguy", false);
static InfantryType TDE2(INFANTRYTD_E2, "e2", "TEXT_UNIT_TITLE_GDI_GRENADIER", "Goodguy", false);
static InfantryType TDE3(INFANTRYTD_E3, "e3", "TEXT_UNIT_TITLE_GDI_ROCKET_SOLDIER", "Badguy", false);
static InfantryType TDE4(INFANTRYTD_E4, "e4", "TEXT_UNIT_TITLE_NOD_FLAMETHROWER", "Badguy", false);
static InfantryType TDE5(INFANTRYTD_E5, "e5", "TEXT_UNIT_TITLE_NOD_CHEM_WARRIOR", "Badguy", false);
static InfantryType TDE7(INFANTRYTD_E7, "e6", "TEXT_UNIT_TITLE_GDI_ENGINEER", "Goodguy", false);
static InfantryType TDCOMMANDO(INFANTRYTD_RAMBO, "rmbo", "TEXT_UNIT_TITLE_GDI_COMMANDO", "Goodguy", false);
static InfantryType TDC1(INFANTRYTD_C1, "c1", "TEXT_UNIT_TITLE_CIV1", "Neutral", false);
static InfantryType TDC2(INFANTRYTD_C2, "c2", "TEXT_UNIT_TITLE_CIV2", "Neutral", false);
static InfantryType TDC3(INFANTRYTD_C3, "c3", "TEXT_UNIT_TITLE_CIV3", "Neutral", false);
static InfantryType TDC4(INFANTRYTD_C4, "c4", "TEXT_UNIT_TITLE_CIV4", "Neutral", false);
static InfantryType TDC5(INFANTRYTD_C5, "c5", "TEXT_UNIT_TITLE_CIV5", "Neutral", false);
static InfantryType TDC6(INFANTRYTD_C6, "c6", "TEXT_UNIT_TITLE_CIV6", "Neutral", false);
static InfantryType TDC7(INFANTRYTD_C7, "c7", "TEXT_UNIT_TITLE_CIV7", "Neutral", false);
static InfantryType TDC8(INFANTRYTD_C8, "c8", "TEXT_UNIT_TITLE_CIV8", "Neutral", false);
static InfantryType TDC9(INFANTRYTD_C9, "c9", "TEXT_UNIT_TITLE_CIV9", "Neutral", false);
static InfantryType TDC10(INFANTRYTD_C10, "c10", "TEXT_UNIT_TITLE_C10", "Neutral", false);
static InfantryType TDMOEBIUS(INFANTRYTD_MOEBIUS, "moebius", "TEXT_UNIT_TITLE_MOEBIUS", "Neutral", false);
static InfantryType TDDELPHI(INFANTRYTD_DELPHI, "delphi", "TEXT_UNIT_TITLE_DELPHI", "Neutral", false);
static InfantryType TDDRCHAN(INFANTRYTD_CHAN, "chan", "TEXT_UNIT_TITLE_CHAN", "Neutral", false);
static InfantryType RAE1(INFANTRYRA_E1, "e1", "TEXT_UNIT_RA_E1", "Greece", true);
static InfantryType RAE2(INFANTRYRA_E2, "e2", "TEXT_UNIT_RA_E2", "USSR", true);
static InfantryType RAE3(INFANTRYRA_E3, "e3", "TEXT_UNIT_RA_E3", "Greece", true);
static InfantryType RAE4(INFANTRYRA_E4, "e4", "TEXT_UNIT_RA_E4", "USSR", true);
static InfantryType RAE6(INFANTRYRA_RENOVATOR, "e6", "TEXT_UNIT_RA_E6", "USSR", true);
static InfantryType RAE7(INFANTRYRA_TANYA, "e7", "TEXT_UNIT_RA_E7", "USSR", true);
static InfantryType RAE8(INFANTRYRA_SPY, "spy", "TEXT_UNIT_RA_SPY", "Greece", true);
static InfantryType RAE9(INFANTRYRA_THIEF, "thf", "TEXT_UNIT_RA_THF", "Greece", true);
static InfantryType RAMEDIC(INFANTRYRA_MEDIC, "medi", "TEXT_UNIT_RA_MEDI", "Greece", true);
static InfantryType RAGENERAL(INFANTRYRA_GENERAL, "gnrl", "TEXT_UNIT_RA_GNRL", "Greece", true);
static InfantryType RADOG(INFANTRYRA_DOG, "dog", "TEXT_UNIT_RA_DOG", "USSR", true);
static InfantryType RAC1(INFANTRYRA_C1, "c1", "TEXT_UNIT_TITLE_CIV1", "Neutral", true);
static InfantryType RAC2(INFANTRYRA_C2, "c2", "TEXT_UNIT_TITLE_CIV2", "Neutral", true);
static InfantryType RAC3(INFANTRYRA_C3, "c3", "TEXT_UNIT_TITLE_CIV3", "Neutral", true);
static InfantryType RAC4(INFANTRYRA_C4, "c4", "TEXT_UNIT_TITLE_CIV4", "Neutral", true);
static InfantryType RAC5(INFANTRYRA_C5, "c5", "TEXT_UNIT_TITLE_CIV5", "Neutral", true);
static InfantryType RAC6(INFANTRYRA_C6, "c6", "TEXT_UNIT_TITLE_CIV6", "Neutral", true);
static InfantryType RAC7(INFANTRYRA_C7, "c7", "TEXT_UNIT_TITLE_CIV7", "Neutral", true);
static InfantryType RAC8(INFANTRYRA_C8, "c8", "TEXT_UNIT_TITLE_CIV8", "Neutral", true);
static InfantryType RAC9(INFANTRYRA_C9, "c9", "TEXT_UNIT_TITLE_CIV9", "Neutral", true);
static InfantryType RAC10(INFANTRYRA_C10, "c10", "TEXT_UNIT_RA_SCIENTIST", "Neutral", true);
static InfantryType RAEINSTEIN(INFANTRYRA_EINSTEIN, "einstein", "TEXT_UNIT_RA_EINSTEIN", "Neutral", true);
static InfantryType RADELPIH(INFANTRYRA_DELPHI, "delphi", "TEXT_UNIT_RA_DELPHI", "Neutral", true);
static InfantryType RADRCHAN(INFANTRYRA_CHAN, "chan", "TEXT_UNIT_TITLE_CHAN", "Neutral", true);
static InfantryType RASHOCKTROOPER(INFANTRYRA_SHOCK, "shok", "TEXT_UNIT_RA_SHOK", "USSR", true);
static InfantryType RAMECHANIC(INFANTRYRA_MECHANIC, "mech", "TEXT_UNIT_RA_MECH", "Greece", true);
void InfantryType::Init(bool isra, HouseType* house, DirectionType direction)
{
	Tile* tile;
	if (TheTilesetManager->GetTileData(Name.c_str(), 0, tile))
	{
		RenderSize = Gdiplus::Size(tile->Image->GetWidth() / TileScale, tile->Image->GetHeight() / TileScale);
	}
	Infantry a;
	a.type = this;
	a.house = house;
	a.strength = 256;
	a.direction = direction;
	Thumbnail = new Gdiplus::Bitmap(TileWidth, TileHeight);
	Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(Thumbnail);
	std::pair<Gdiplus::Rect, RenderFunc> v = MapRender::Render(isra, Gdiplus::Point(1, 1), Gdiplus::Size(TileWidth, TileHeight), &a, STOPPING_CENTER);
	v.second(g, v.first);
	delete g;
}
