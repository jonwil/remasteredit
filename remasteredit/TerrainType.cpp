#include "global.h"
#include "TerrainType.h"
#include "theatertype.h"
#include "tileset.h"
std::map<std::string, TerrainType*> TerrainType::TerrainMapTD;
std::map<std::string, TerrainType*> TerrainType::TerrainMapRA;
static bool mask1[4] = { false, false, true, false };
static bool mask2[1] = { true };
static bool mask3[2] = { true, false };
static bool mask4[4] = { false, false, true, true };
static bool mask5[6] = { false, false, false, false, true, false };
static bool mask6[6] = { false, false, false, true, true, false };
static bool mask7[6] = { false, true, false, true, true, false };
static bool mask8[6] = { true, true, false, true, true, false };
static bool mask9[12] = { false, false, false, false, true, true, true, false, true, false, false, false };
static bool mask10[12] = { false, false, true, false, true, true, true, false, false, true, true, false };
static bool mask11[4] = { true, true, false, false };
static bool mask12[6] = { false, false, false, true, true, true };
static bool mask13[5] = { true, true, true, true, false };
static bool mask14[4] = { true, true, true, true };
static bool mask15[4] = { true, false, true, false };
static bool mask16[2] = { true, true };
static TerrainType TDTREE1(TERRAINTD_TREE1, "t01", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(2, 2), mask1, false);
static TerrainType TDTREE2(TERRAINTD_TREE2, "t02", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(2, 2), mask1, false);
static TerrainType TDTREE3(TERRAINTD_TREE3, "t03", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(2, 2), mask1, false);
static TerrainType TDTREE4(TERRAINTD_TREE4, "t04", THEATERF_DESERTTD, false, false, Gdiplus::Size(1, 1), mask2, false);
static TerrainType TDTREE5(TERRAINTD_TREE5, "t05", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(2, 2), mask1, false);
static TerrainType TDTREE6(TERRAINTD_TREE6, "t06", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(2, 2), mask1, false);
static TerrainType TDTREE7(TERRAINTD_TREE7, "t07", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(2, 2), mask1, false);
static TerrainType TDTREE8(TERRAINTD_TREE8, "t08", THEATERF_WINTERTD | THEATERF_TEMPERATETD | THEATERF_DESERTTD, false, false, Gdiplus::Size(2, 1), mask3, false);
static TerrainType TDTREE9(TERRAINTD_TREE9, "t09", THEATERF_DESERTTD, false, false, Gdiplus::Size(2, 1), mask3, false);
static TerrainType TDTREE10(TERRAINTD_TREE10, "t10", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(2, 2), mask4, false);
static TerrainType TDTREE11(TERRAINTD_TREE11, "t11", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(2, 2), mask4, false);
static TerrainType TDTREE12(TERRAINTD_TREE12, "t12", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(2, 2), mask1, false);
static TerrainType TDTREE13(TERRAINTD_TREE13, "t13", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(2, 2), mask1, false);
static TerrainType TDTREE14(TERRAINTD_TREE14, "t14", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(2, 2), mask4, false);
static TerrainType TDTREE15(TERRAINTD_TREE15, "t15", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(2, 2), mask4, false);
static TerrainType TDTREE16(TERRAINTD_TREE16, "t16", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(2, 2), mask1, false);
static TerrainType TDTREE17(TERRAINTD_TREE17, "t17", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(2, 2), mask1, false);
static TerrainType TDTREE18(TERRAINTD_TREE18, "t18", THEATERF_DESERTTD, false, false, Gdiplus::Size(3, 2), mask5, false);
static TerrainType TDSPLIT1(TERRAINTD_BLOSSOMTREE1, "split2", THEATERF_TEMPERATETD | THEATERF_WINTERTD, true, false, Gdiplus::Size(2, 2), mask1, false);
static TerrainType TDSPLIT2(TERRAINTD_BLOSSOMTREE2, "split3", THEATERF_TEMPERATETD | THEATERF_WINTERTD | THEATERF_DESERTTD, true, false, Gdiplus::Size(2, 2), mask1, false);
static TerrainType TDCLUMP1(TERRAINTD_CLUMP1, "tc01", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(3, 2), mask6, false);
static TerrainType TDCLUMP2(TERRAINTD_CLUMP2, "tc02", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(3, 2), mask7, false);
static TerrainType TDCLUMP3(TERRAINTD_CLUMP3, "tc03", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(3, 2), mask8, false);
static TerrainType TDCLUMP4(TERRAINTD_CLUMP4, "tc04", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(4, 3), mask9, false);
static TerrainType TDCLUMP5(TERRAINTD_CLUMP5, "tc05", THEATERF_WINTERTD | THEATERF_TEMPERATETD, false, false, Gdiplus::Size(4, 3), mask10, false);
static TerrainType TDROCK1(TERRAINTD_ROCK1, "rock1", THEATERF_DESERTTD, false, false, Gdiplus::Size(2, 2), mask4, false);
static TerrainType TDROCK2(TERRAINTD_ROCK2, "rock2", THEATERF_DESERTTD, false, false, Gdiplus::Size(2, 2), mask11, false);
static TerrainType TDROCK3(TERRAINTD_ROCK3, "rock3", THEATERF_DESERTTD, false, false, Gdiplus::Size(3, 2), mask6, false);
static TerrainType TDROCK4(TERRAINTD_ROCK4, "rock4", THEATERF_DESERTTD, false, false, Gdiplus::Size(2, 1), mask3, false);
static TerrainType TDROCK5(TERRAINTD_ROCK5, "rock5", THEATERF_DESERTTD, false, false, Gdiplus::Size(2, 1), mask3, false);
static TerrainType TDROCK6(TERRAINTD_ROCK6, "rock6", THEATERF_DESERTTD, false, false, Gdiplus::Size(3, 2), mask12, false);
static TerrainType TDROCK7(TERRAINTD_ROCK7, "rock7", THEATERF_DESERTTD, false, false, Gdiplus::Size(5, 1), mask13, false);
static TerrainType RATREE1(TERRAINRA_TREE1, "t01", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 2), mask1, true);
static TerrainType RATREE2(TERRAINRA_TREE2, "t02", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 2), mask1, true);
static TerrainType RATREE3(TERRAINRA_TREE3, "t03", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 2), mask1, true);
static TerrainType RATREE5(TERRAINRA_TREE5, "t05", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 2), mask1, true);
static TerrainType RATREE6(TERRAINRA_TREE6, "t06", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 2), mask1, true);
static TerrainType RATREE7(TERRAINRA_TREE7, "t07", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 2), mask1, true);
static TerrainType RATREE8(TERRAINRA_TREE8, "t08", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 1), mask3, true);
static TerrainType RATREE10(TERRAINRA_TREE10, "t10", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 2), mask4, true);
static TerrainType RATREE11(TERRAINRA_TREE11, "t11", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 2), mask4, true);
static TerrainType RATREE12(TERRAINRA_TREE12, "t12", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 2), mask1, true);
static TerrainType RATREE13(TERRAINRA_TREE13, "t13", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 2), mask1, true);
static TerrainType RATREE14(TERRAINRA_TREE14, "t14", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 2), mask4, true);
static TerrainType RATREE15(TERRAINRA_TREE15, "t15", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 2), mask4, true);
static TerrainType RATREE16(TERRAINRA_TREE16, "t16", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 2), mask1, true);
static TerrainType RATREE17(TERRAINRA_TREE17, "t17", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 2), mask1, true);
static TerrainType RACLUMP1(TERRAINRA_CLUMP1, "tc01", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(3, 2), mask6, true);
static TerrainType RACLUMP2(TERRAINRA_CLUMP2, "tc02", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(3, 2), mask7, true);
static TerrainType RACLUMP3(TERRAINRA_CLUMP3, "tc03", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(3, 2), mask8, true);
static TerrainType RACLUMP4(TERRAINRA_CLUMP4, "tc04", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(4, 3), mask9, true);
static TerrainType RACLUMP5(TERRAINRA_CLUMP5, "tc05", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, false, Gdiplus::Size(4, 3), mask10, true);
static TerrainType RAICE01(TERRAINRA_ICE01, "ice01", THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 2), mask14, true);
static TerrainType RAICE02(TERRAINRA_ICE02, "ice02", THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 2), mask15, true);
static TerrainType RAICE03(TERRAINRA_ICE03, "ice03", THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 1), mask16, true);
static TerrainType RAICE04(TERRAINRA_ICE04, "ice04", THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 1), mask3, true);
static TerrainType RAICE05(TERRAINRA_ICE05, "ice05", THEATERF_SNOWRA, false, false, Gdiplus::Size(2, 1), mask3, true);
static TerrainType RABOXES01(TERRAINRA_BOXES01, "boxes01", THEATERF_INTERIORRA, false, false, Gdiplus::Size(2, 1), mask3, true);
static TerrainType RABOXES02(TERRAINRA_BOXES02, "boxes02", THEATERF_INTERIORRA, false, false, Gdiplus::Size(2, 1), mask3, true);
static TerrainType RABOXES03(TERRAINRA_BOXES03, "boxes03", THEATERF_INTERIORRA, false, false, Gdiplus::Size(2, 1), mask3, true);
static TerrainType RABOXES04(TERRAINRA_BOXES04, "boxes04", THEATERF_INTERIORRA, false, false, Gdiplus::Size(2, 1), mask3, true);
static TerrainType RABOXES05(TERRAINRA_BOXES05, "boxes05", THEATERF_INTERIORRA, false, false, Gdiplus::Size(2, 1), mask3, true);
static TerrainType RABOXES06(TERRAINRA_BOXES06, "boxes06", THEATERF_INTERIORRA, false, false, Gdiplus::Size(2, 1), mask3, true);
static TerrainType RABOXES07(TERRAINRA_BOXES07, "boxes07", THEATERF_INTERIORRA, false, false, Gdiplus::Size(2, 1), mask3, true);
static TerrainType RABOXES08(TERRAINRA_BOXES08, "boxes08", THEATERF_INTERIORRA, false, false, Gdiplus::Size(2, 1), mask3, true);
static TerrainType RABOXES09(TERRAINRA_BOXES09, "boxes09", THEATERF_INTERIORRA, false, false, Gdiplus::Size(2, 1), mask3, true);
static TerrainType RAMINE(TERRAINRA_MINE, "mine", THEATERF_TEMPERATERA | THEATERF_SNOWRA, false, true, Gdiplus::Size(2, 1), mask3, true);

void TerrainType::Init()
{
	std::string name = Name;
	if (IsMine)
	{
		name = "OREMINE";
	}
	Tile* tile;
	if (TheTilesetManager->GetTileData(name.c_str(), IsTransformable ? 22 : 0, tile))
	{
		RenderSize = Gdiplus::Size(tile->Image->GetWidth() / TileScale, tile->Image->GetHeight() / TileScale);
		Thumbnail = tile->Image->Clone(Gdiplus::Rect(0, 0, tile->Image->GetWidth() / 2, tile->Image->GetHeight() / 2), PixelFormatDontCare);
		OverlapBounds = Gdiplus::Rect(Gdiplus::Point(0, 0), Gdiplus::Size((RenderSize.Width + TileWidth - 1) / TileWidth, (RenderSize.Height + TileHeight - 1) / TileHeight));
	}
}
