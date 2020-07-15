#include "global.h"
#include "AircraftType.h"
#include "map.h"
#include "maprender.h"
#include "tileset.h"
std::map<std::string, AircraftType*> AircraftType::AircraftMapTD;
std::map<std::string, AircraftType*> AircraftType::AircraftMapRA;
static AircraftType TDTRAN(AIRCRAFTTD_TRANSPORT, "tran", "TEXT_UNIT_TITLE_GDI_TRANSPORT", "Goodguy", 2, false);
static AircraftType TDA10(AIRCRAFTTD_A10, "a10", "TEXT_UNIT_TITLE_A10", "Goodguy", 0, false);
static AircraftType TDHELI(AIRCRAFTTD_HELICOPTER, "heli", "TEXT_UNIT_TITLE_NOD_HELICOPTER", "Badguy", 1, false);
static AircraftType TDC17(AIRCRAFTTD_CARGO, "c17", "TEXT_UNIT_TITLE_C17", "Badguy", 0, false);
static AircraftType TDORCA(AIRCRAFTTD_ORCA, "orca", "TEXT_UNIT_TITLE_GDI_ORCA", "Goodguy", 0, false);
static AircraftType RATRAN(AIRCRAFTRA_TRANSPORT, "tran", "TEXT_UNIT_RA_TRAN", "USSR", 2, true);
static AircraftType RABADR(AIRCRAFTRA_BADGER, "badr", "TEXT_UNIT_RA_BADR", "USSR", 0, true);
static AircraftType RAU2(AIRCRAFTRA_U2, "u2", "TEXT_UNIT_RA_U2", "USSR", 0, true);
static AircraftType RAMIG(AIRCRAFTRA_MIG, "mig", "TEXT_UNIT_RA_MIG", "USSR", 0, true);
static AircraftType RAYAK(AIRCRAFTRA_YAK, "yak", "TEXT_UNIT_RA_YAK", "USSR", 0, true);
static AircraftType RAHELI(AIRCRAFTRA_LONGBOW, "heli", "TEXT_UNIT_RA_HELI", "Greece", 1, true);
static AircraftType RAHIND(AIRCRAFTRA_HIND, "hind", "TEXT_UNIT_RA_HIND", "USSR", 1, true);
void AircraftType::Init(bool isra, HouseType* house, DirectionType direction)
{
	Tile* tile;
	if (TheTilesetManager->GetTileData(Name.c_str(), 0, tile))
	{
		RenderSize = Gdiplus::Size(tile->Image->GetWidth() / TileScale, tile->Image->GetHeight() / TileScale);
	}
	Aircraft a;
	a.type = this;
	a.house = house;
	a.strength = 256;
	a.direction = direction;
	Thumbnail = new Gdiplus::Bitmap(TileWidth * 3, TileHeight * 3);
	Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(Thumbnail);
	std::pair<Gdiplus::Rect, RenderFunc> v = MapRender::Render(isra, Gdiplus::Point(1, 1), Gdiplus::Size(TileWidth, TileHeight), &a);
	v.second(g, v.first);
	delete g;
}
