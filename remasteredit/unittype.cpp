#include "global.h"
#include "unittype.h"
#include "map.h"
#include "maprender.h"
#include "tileset.h"
std::map<std::string, UnitType*> UnitType::UnitMapTD;
std::map<std::string, UnitType*> UnitType::UnitMapRA;
static UnitType TDHTANK(UNITTD_HTANK, "htnk", "TEXT_UNIT_TITLE_GDI_MAMMOTH_TANK", "Goodguy", true, false, false, false, false, false);
static UnitType TDMTANK(UNITTD_MTANK, "mtnk", "TEXT_UNIT_TITLE_GDI_MED_TANK", "Goodguy", true, false, false, false, false, false);
static UnitType TDLTANK(UNITTD_LTANK, "ltnk", "TEXT_UNIT_TITLE_NOD_LIGHT_TANK", "Badguy", true, false, false, false, false, false);
static UnitType TDSTANK(UNITTD_STANK, "stnk", "TEXT_UNIT_TITLE_NOD_STEALTH_TANK", "Badguy", false, false, false, false, false, false);
static UnitType TDFTANK(UNITTD_FTANK, "ftnk", "TEXT_UNIT_TITLE_NOD_FLAME_TANK", "Badguy", false, false, false, false, false, false);
static UnitType TDVISCEROID(UNITTD_VICE, "vice", "TEXT_UNIT_TITLE_VICE", "Special", false, false, false, false, true, false);
static UnitType TDAPC(UNITTD_APC, "apc", "TEXT_UNIT_TITLE_GDI_APC", "Goodguy", false, false, false, false, false, false);
static UnitType TDMLRS(UNITTD_MLRS, "msam", "TEXT_UNIT_TITLE_GDI_MRLS", "Goodguy", true, false, false, false, false, false);
static UnitType TDJEEP(UNITTD_JEEP, "jeep", "TEXT_UNIT_TITLE_GDI_HUMVEE", "Goodguy", true, false, false, false, false, false);
static UnitType TDBUGGY(UNITTD_BUGGY, "bggy", "TEXT_UNIT_TITLE_NOD_NOD_BUGGY", "Badguy", true, false, false, false, false, false);
static UnitType TDHARVESTER(UNITTD_HARVESTER, "harv", "TEXT_UNIT_TITLE_GDI_HARVESTER", "Goodguy", false, false, false, false, false, false);
static UnitType TDARTY(UNITTD_ARTY, "arty", "TEXT_UNIT_TITLE_NOD_ARTILLERY", "Badguy", false, false, false, false, false, false);
static UnitType TDSAM(UNITTD_MSAM, "mlrs", "TEXT_UNIT_TITLE_GDI_MLRS", "Goodguy", true, false, false, false, false, false);
static UnitType TDHOVER(UNITTD_HOVER, "lst", "TEXT_UNIT_TITLE_LST", "Goodguy", false, false, false, true, false, false);
static UnitType TDMHQ(UNITTD_MHQ, "mhq", "TEXT_UNIT_TITLE_MHQ", "Goodguy", false, true, false, false, false, false);
static UnitType TDGUNBOAT(UNITTD_GUNBOAT, "boat", "TEXT_UNIT_TITLE_WAKE", "Goodguy", true, false, false, true, false, false);
static UnitType TDMCV(UNITTD_MCV, "mcv", "TEXT_UNIT_TITLE_GDI_MCV", "Goodguy", false, false, false, false, false, false);
static UnitType TDBIKE(UNITTD_BIKE, "bike", "TEXT_UNIT_TITLE_NOD_RECON_BIKE", "Badguy", false, false, false, false, false, false);
static UnitType TDTRIC(UNITTD_TRIC, "tric", "TEXT_UNIT_TITLE_TRIC", "Special", false, false, true, false, false, false);
static UnitType TDTREX(UNITTD_TREX, "trex", "TEXT_UNIT_TITLE_TREX", "Special", false, false, true, false, false, false);
static UnitType TDRAPT(UNITTD_RAPT, "rapt", "TEXT_UNIT_TITLE_RAPT", "Special", false, false, true, false, false, false);
static UnitType TDSTEG(UNITTD_STEG, "steg", "TEXT_UNIT_TITLE_STEG", "Special", false, false, true, false, false, false);
static UnitType RAHTANK(UNITRA_HTANK, "4tnk", "TEXT_UNIT_RA_4TNK", "USSR", true, false, false, false, false, true);
static UnitType RAMTANK(UNITRA_MTANK, "3tnk", "TEXT_UNIT_RA_3TNK", "USSR", true, false, false, false, false, true);
static UnitType RAMTANK2(UNITRA_MTANK2, "2tnk", "TEXT_UNIT_RA_2TNK", "Greece", true, false, false, false, false, true);
static UnitType RALTANK(UNITRA_LTANK, "1tnk", "TEXT_UNIT_RA_1TNK", "Greece", true, false, false, false, false, true);
static UnitType RAAPC(UNITRA_APC, "apc", "TEXT_UNIT_RA_APC", "Greece", false, false, false, false, false, true);
static UnitType RAMINELAYER(UNITRA_MINELAYER, "mnly", "TEXT_UNIT_RA_MNLY", "Greece", false, false, false, false, false, true);
static UnitType RAJEEP(UNITRA_JEEP, "jeep", "TEXT_UNIT_RA_JEEP", "Greece", true, false, false, false, false, true);
static UnitType RAHARVESTER(UNITRA_HARVESTER, "harv", "TEXT_UNIT_RA_HARV", "Greece", false, false, false, false, false, true);
static UnitType RAARTY(UNITRA_ARTY, "arty", "TEXT_UNIT_RA_ARTY", "Greece", false, false, false, false, false, true);
static UnitType RAMRJAMMER(UNITRA_MRJ, "mrj", "TEXT_UNIT_RA_MRJ", "Greece", false, true, false, false, false, true);
static UnitType RAMGG(UNITRA_MGG, "mgg", "TEXT_UNIT_RA_MGG", "Greece", false, true, false, false, false, true);
static UnitType RAMCV(UNITRA_MCV, "mcv", "TEXT_UNIT_RA_MCV", "Greece", false, false, false, false, false, true);
static UnitType RAV2LAUNCHER(UNITRA_V2_LAUNCHER, "v2rl", "TEXT_UNIT_RA_V2RL", "USSR", false, false, false, false, false, true);
static UnitType RACONVOYTRUCK(UNITRA_TRUCK, "truk", "TEXT_UNIT_RA_TRUK", "Greece", false, false, false, false, false, true);
static UnitType RAANT1(UNITRA_ANT1, "ant1", "TEXT_UNIT_RA_ANT1", "Special", false, false, true, false, false, true);
static UnitType RAANT2(UNITRA_ANT2, "ant2", "TEXT_UNIT_RA_ANT2", "Special", false, false, true, false, false, true);
static UnitType RAANT3(UNITRA_ANT3, "ant3", "TEXT_UNIT_RA_ANT3", "Special", false, false, true, false, false, true);
static UnitType RACHRONO(UNITRA_CHRONOTANK, "ctnk", "TEXT_UNIT_RA_CTNK", "Greece", false, false, false, false, false, true);
static UnitType RATESLA(UNITRA_TESLATANK, "ttnk", "TEXT_UNIT_RA_TTNK", "USSR", false, true, false, false, false, true);
static UnitType RAMAD(UNITRA_MAD, "qtnk", "TEXT_UNIT_RA_QTNK", "Greece", false, false, false, false, false, true);
static UnitType RADEMOTRUCK(UNITRA_DEMOTRUCK, "dtrk", "TEXT_UNIT_RA_DTRK", "USSR", false, false, false, false, false, true);
static UnitType RAPHASE(UNITRA_PHASE, "stnk", "TEXT_UNIT_RA_STNK", "Greece", true, false, false, false, false, true);
void UnitType::Init(bool isra, HouseType* house, DirectionType direction)
{
	Tile* tile;
	if (TheTilesetManager->GetTileData(Name.c_str(), 0, tile))
	{
		RenderSize = Gdiplus::Size(tile->Image->GetWidth() / TileScale, tile->Image->GetHeight() / TileScale);
	}
	Unit a;
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
