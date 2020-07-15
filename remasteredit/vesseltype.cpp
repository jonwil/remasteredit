#include "global.h"
#include "vesseltype.h"
#include "map.h"
#include "maprender.h"
#include "tileset.h"
std::map<std::string, VesselType*> VesselType::VesselMapTD;
std::map<std::string, VesselType*> VesselType::VesselMapRA;
static VesselType RASUBMARINE(VESSELRA_SS, "ss", "TEXT_UNIT_RA_SS", "USSR", false, true);
static VesselType RADESTROYER(VESSELRA_DD, "dd", "TEXT_UNIT_RA_DD", "Greece", true, true);
static VesselType RACRUISER(VESSELRA_CA, "ca", "TEXT_UNIT_RA_CA", "Greece", true, true);
static VesselType RATRANSPORT(VESSELRA_TRANSPORT, "lst", "TEXT_UNIT_RA_LST", "Greece", false, true);
static VesselType RAPTBOAT(VESSELRA_PT, "pt", "TEXT_UNIT_RA_PT", "Greece", true, true);
static VesselType RAMISSILESUBMARINE(VESSELRA_MISSILESUB, "msub", "TEXT_UNIT_RA_MSUB", "USSR", false, true);
static VesselType RACARRIER(VESSELRA_CARRIER, "carr", "TEXT_UNIT_RA_CARR", "Greece", false, true);
void VesselType::Init(bool isra, HouseType* house, DirectionType direction)
{
	Tile* tile;
	if (TheTilesetManager->GetTileData(Name.c_str(), 0, tile))
	{
		RenderSize = Gdiplus::Size(tile->Image->GetWidth() / TileScale, tile->Image->GetHeight() / TileScale);
	}
	Vessel a;
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
