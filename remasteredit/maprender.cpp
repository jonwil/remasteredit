#include "global.h"
#include "maprender.h"
#include "tileset.h"
#include "map.h"
#include "teamcolor.h"
#include "structtype.h"
#include "housetype.h"
#include "overlaytype.h"
#include "smudgetype.h"
#include "TemplateType.h"
#include "TerrainType.h"
#include "AircraftType.h"
#include "vesseltype.h"
#include "boolinq.h"
static unsigned short _stretch[FACING_COUNT] = { 8, 9, 10, 9, 8, 9, 10, 9 };
static int Facing16[256] =
{
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	7,
	7,
	7,
	7,
	7,
	7,
	7,
	7,
	7,
	7,
	7,
	7,
	7,
	7,
	7,
	7,
	8,
	8,
	8,
	8,
	8,
	8,
	8,
	8,
	8,
	8,
	8,
	8,
	8,
	8,
	8,
	8,
	9,
	9,
	9,
	9,
	9,
	9,
	9,
	9,
	9,
	9,
	9,
	9,
	9,
	9,
	9,
	9,
	10,
	10,
	10,
	10,
	10,
	10,
	10,
	10,
	10,
	10,
	10,
	10,
	10,
	10,
	10,
	10,
	11,
	11,
	11,
	11,
	11,
	11,
	11,
	11,
	11,
	11,
	11,
	11,
	11,
	11,
	11,
	11,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	14,
	14,
	14,
	14,
	14,
	14,
	14,
	14,
	14,
	14,
	14,
	14,
	14,
	14,
	14,
	14,
	15,
	15,
	15,
	15,
	15,
	15,
	15,
	15,
	15,
	15,
	15,
	15,
	15,
	15,
	15,
	15,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};
static int Facing32[256] =
{
	0,
	0,
	0,
	0,
	0,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	4,
	4,
	4,
	4,
	4,
	4,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	7,
	7,
	7,
	7,
	7,
	7,
	7,
	8,
	8,
	8,
	8,
	8,
	8,
	8,
	9,
	9,
	9,
	9,
	9,
	9,
	9,
	10,
	10,
	10,
	10,
	10,
	10,
	10,
	11,
	11,
	11,
	11,
	11,
	11,
	11,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	14,
	14,
	14,
	14,
	14,
	14,
	14,
	14,
	14,
	15,
	15,
	15,
	15,
	15,
	15,
	15,
	15,
	15,
	16,
	16,
	16,
	16,
	16,
	16,
	16,
	16,
	16,
	16,
	16,
	17,
	17,
	17,
	17,
	17,
	17,
	17,
	17,
	17,
	18,
	18,
	18,
	18,
	18,
	18,
	18,
	18,
	18,
	19,
	19,
	19,
	19,
	19,
	19,
	19,
	19,
	19,
	19,
	20,
	20,
	20,
	20,
	20,
	20,
	21,
	21,
	21,
	21,
	21,
	21,
	21,
	22,
	22,
	22,
	22,
	22,
	22,
	22,
	23,
	23,
	23,
	23,
	23,
	23,
	23,
	24,
	24,
	24,
	24,
	24,
	24,
	24,
	25,
	25,
	25,
	25,
	25,
	25,
	25,
	26,
	26,
	26,
	26,
	26,
	26,
	26,
	27,
	27,
	27,
	27,
	27,
	27,
	27,
	28,
	28,
	28,
	28,
	28,
	28,
	28,
	28,
	29,
	29,
	29,
	29,
	29,
	29,
	29,
	29,
	30,
	30,
	30,
	30,
	30,
	30,
	30,
	30,
	30,
	31,
	31,
	31,
	31,
	31,
	31,
	31,
	31,
	31,
	0,
	0,
	0,
	0,
	0,
	0
};
static int HumanShape[32] =
{
	0,
	0,
	7,
	7,
	7,
	7,
	6,
	6,
	6,
	6,
	5,
	5,
	5,
	5,
	5,
	4,
	4,
	4,
	3,
	3,
	3,
	3,
	2,
	2,
	2,
	2,
	1,
	1,
	1,
	1,
	1,
	0
};
static int BodyShape[32] =
{
	0,
	31,
	30,
	29,
	28,
	27,
	26,
	25,
	24,
	23,
	22,
	21,
	20,
	19,
	18,
	17,
	16,
	15,
	14,
	13,
	12,
	11,
	10,
	9,
	8,
	7,
	6,
	5,
	4,
	3,
	2,
	1
};
static Gdiplus::Point TurretAdjust[32] =
{
	{ 1, 2 },
	{ -1, 1 },
	{ -2, 0 },
	{ -3, 0 },
	{ -3, 1 },
	{ -4, -1 },
	{ -4, -1 },
	{ -5, -2 },
	{ -5, -3 },
	{ -5, -3 },
	{ -3, -3 },
	{ -3, -4 },
	{ -3, -4 },
	{ -3, -5 },
	{ -2, -5 },
	{ -1, -5 },
	{ 0, -5 },
	{ 1, -6 },
	{ 2, -5 },
	{ 3, -5 },
	{ 4, -5 },
	{ 6, -4 },
	{ 6, -3 },
	{ 6, -3 },
	{ 6, -3 },
	{ 5, -1 },
	{ 5, -1 },
	{ 4, 0 },
	{ 3, 0 },
	{ 2, 0 },
	{ 2, 1 },
	{ 1, 2 }
};
static int tiberiumCounts[9] =
{
	0,
	1,
	3,
	4,
	6,
	7,
	8,
	10,
	11
};
void MapRender::Render(Map* map, Gdiplus::Graphics* graphics, const std::set<Gdiplus::Point>& locations, MapLayerFlag layers, int tileScale)
{
	Gdiplus::Size tileSize(128 / tileScale, 128 / tileScale);
	std::vector<std::pair<Gdiplus::Rect, RenderFunc>> list;
	using cellfunc = std::function<const std::set<Gdiplus::Point>()>;
	cellfunc func;
	if (!locations.empty())
	{
		func = [locations]() ->const std::set<Gdiplus::Point> {return locations; };
	}
	else
	{
		func = [map]() ->const std::set<Gdiplus::Point>
		{
			std::set<Gdiplus::Point> v;
			for (int y = 0; y < map->metrics->Height; y++)
			{
				for (int x = 0; x < map->metrics->Width; x++)
				{
					v.insert(Gdiplus::Point(x, y));
				}
			}
			return v;
		};
	}
	if (layers & MAPLAYER_TEMPLATE)
	{
		for (auto item9 : func())
		{
			int cell;
			map->metrics->GetCell(item9, cell);
			Template *t = map->templates->Get(cell);
			const char* name = "clear1";
			int shape = ((cell & 3) | ((cell >> 4) & 12));
			if (t)
			{
				name = t->type->Name.c_str();
				shape = t->icon;
			}
			Tile* tile;
			if (TheTilesetManager->GetTileData(name, shape, tile))
			{
				Gdiplus::Rect r(item9.X * tileSize.Width, item9.Y * tileSize.Height, tileSize.Width, tileSize.Height);
				graphics->DrawImage(tile->Image, r);
			}
		}
	}
	if (layers & MAPLAYER_SMUDGE)
	{
		for (auto item10 : func())
		{
			Smudge *smudge = map->smudges->Get(item10);
			if (smudge != nullptr)
			{
				auto r = Render(item10, tileSize, tileScale, smudge);
				r.second(graphics,r.first);
			}
		}
	}
	if (layers & MAPLAYER_OVERLAYALL)
	{
		for (auto item11 : func())
		{
			Overlay *overlay = map->overlays->Get(item11);
			if (overlay != nullptr && ((overlay->type->IsResource && (layers & MAPLAYER_RESOURCES) != 0) || (overlay->type->IsWall && (layers & MAPLAYER_WALLS) != 0) || (layers & MAPLAYER_OVERLAY) != 0))
			{
				auto r = Render(map->isra, item11, tileSize, tileScale, overlay);
				r.second(graphics, r.first);
			}
		}
	}
	if (layers & MAPLAYER_TERRAIN)
	{
		for (auto i : map->technos->GetOccupiers<Terrain>())
		{
			Terrain* o = i.first;
			Gdiplus::Point p = i.second;
			const char* name = nullptr;
			int icon = 0;
			if (o->type)
			{
				name = o->type->Name.c_str();
				if (o->type->IsMine)
				{
					name = "OREMINE";
				}
			}
			if (name)
			{
				Tile* tile;
				if (TheTilesetManager->GetTileData(name, icon, tile))
				{
					Gdiplus::Point location(p.X * tileSize.Width, p.Y * tileSize.Height);
					Gdiplus::Size size(tile->Image->GetWidth() / tileScale, tile->Image->GetHeight() / tileScale);
					Gdiplus::Rect terrainBounds(location, size);
					list.push_back(std::make_pair(terrainBounds, [tile](Gdiplus::Graphics* g, Gdiplus::Rect terrainBounds) {g->DrawImage(tile->Image, terrainBounds, 0, 0, tile->Image->GetWidth(), tile->Image->GetHeight(), Gdiplus::UnitPixel); }));
				}
			}
		}
	}
	if ((layers & MAPLAYER_BUILDINGS) != 0)
	{
		for (auto i : map->technos->GetOccupiers<Structure>())
		{
			Structure* building = i.first;
			Gdiplus::Point topLeft = i.second;
			if (locations.empty() || locations.count(topLeft))
			{
				list.push_back(Render(map->isra, topLeft, tileSize, tileScale, building));
			}
		}
	}
	if ((layers & MAPLAYER_UNITS) != 0)
	{
		for (auto i : map->technos->GetOccupiers<Unit>())
		{
			Aircraft* aircraft = dynamic_cast<Aircraft*>(i.first);
			if (aircraft)
			{
				Gdiplus::Point topLeft = i.second;
				if (locations.empty() || locations.count(topLeft))
				{
					list.push_back(Render(map->isra, topLeft, tileSize, aircraft));
				}
			}
			Vessel* vessel = dynamic_cast<Vessel*>(i.first);
			if (vessel)
			{
				Gdiplus::Point topLeft = i.second;
				if (locations.empty() || locations.count(topLeft))
				{
					list.push_back(Render(map->isra, topLeft, tileSize, vessel));
				}
			}
		}
	}
	from(list).where([](std::pair<Gdiplus::Rect, RenderFunc> x) {return !x.first.Equals(Gdiplus::Rect()); }).orderBy([](std::pair<Gdiplus::Rect, RenderFunc> x) {return x.first.GetBottom(); }).for_each([graphics](std::pair<Gdiplus::Rect, RenderFunc> item16) { item16.second(graphics, item16.first); });
}

void MapRender::Render(Map* map, Gdiplus::Graphics* graphics, const std::set<Gdiplus::Point> &locations, MapLayerFlag layers)
{
	Render(map, graphics, locations, layers, TileScale);
}

std::pair<Gdiplus::Rect, RenderFunc> MapRender::Render(Gdiplus::Point topLeft, Gdiplus::Size tileSize, int tileScale, Smudge* smudge)
{
	const char* name = nullptr;
	int icon = 0;
	Gdiplus::Rect smudgeBounds;
	if (smudge->type)
	{
		name = smudge->type->Name.c_str();
		icon = smudge->icon;
	}
	if (name)
	{
		Tile* tile;
		if (TheTilesetManager->GetTileData(name, icon, tile))
		{
			Gdiplus::Point location = Gdiplus::Point(topLeft.X * tileSize.Width, topLeft.Y * tileSize.Height);
			Gdiplus::Size size(tile->Image->GetWidth() / tileScale, tile->Image->GetHeight() / tileScale);
			smudgeBounds = Gdiplus::Rect(location, size);
			return std::make_pair(smudgeBounds, [tile](Gdiplus::Graphics* graphics, Gdiplus::Rect smudgeBounds)
			{
				graphics->DrawImage(tile->Image, smudgeBounds, 0, 0, tile->Image->GetWidth(), tile->Image->GetHeight(), Gdiplus::UnitPixel);
			});
		}
	}
	return std::make_pair(Gdiplus::Rect(), [](Gdiplus::Graphics*, Gdiplus::Rect) {});
}

std::pair<Gdiplus::Rect, RenderFunc> MapRender::Render(bool isra, Gdiplus::Point topLeft, Gdiplus::Size tileSize, int tileScale, Overlay* overlay)
{
	const char* name = nullptr;
	int icon = 0;
	Gdiplus::Rect overlayBounds;
	if (overlay->type)
	{
		name = overlay->type->Name.c_str();
		if (overlay->type->Flag == OVERLAYTYPE_TIBERIUMORGOLD && !isra)
		{
			name = OverlayType::Tiberium[rand() % OverlayType::Tiberium.size()]->Name.c_str();
		}
		if (overlay->type->Flag == OVERLAYTYPE_TIBERIUMORGOLD && isra)
		{
			name = OverlayType::Gold[rand() % OverlayType::Gold.size()]->Name.c_str();
		}
		if (overlay->type->Flag == OVERLAYTYPE_GEMS)
		{
			name = OverlayType::Gems[rand() % OverlayType::Gems.size()]->Name.c_str();
		}
		icon = overlay->icon;
	}
	if (name)
	{
		Tile* tile;
		if (TheTilesetManager->GetTileData(name, icon, tile))
		{
			Gdiplus::Size size(tileSize);
			if (overlay->type->Flag == OVERLAYTYPE_CRATE || overlay->type->Flag == OVERLAYTYPE_FLAG)
			{
				size = Gdiplus::Size(tile->Image->GetWidth() / tileScale, tile->Image->GetHeight() / tileScale);
			}
			Gdiplus::Point location = Gdiplus::Point(topLeft.X * tileSize.Width, topLeft.Y * tileSize.Height) + Gdiplus::Size(tileSize.Width / 2, tileSize.Height / 2) - Gdiplus::Size(size.Width / 2, size.Height / 2);
			overlayBounds = Gdiplus::Rect(location, size);
			return std::make_pair(overlayBounds, [tile](Gdiplus::Graphics* graphics, Gdiplus::Rect overlayBounds)
			{
				graphics->DrawImage(tile->Image, overlayBounds, 0, 0, tile->Image->GetWidth(), tile->Image->GetHeight(), Gdiplus::UnitPixel);
			});
		}
	}
	return std::make_pair(Gdiplus::Rect(), [](Gdiplus::Graphics*, Gdiplus::Rect) {});
}

std::pair<Gdiplus::Rect, RenderFunc> MapRender::Render(bool isra, Gdiplus::Point topLeft, Gdiplus::Size tileSize, int tileScale, Structure* building)
{
	int num = 0;
	if (building->type->HasTurret)
	{
		num = BodyShape[Facing32[building->direction.ID]];
		if (building->strength < 128)
		{
			if (isra)
			{
				num += (building->type->ID == STRUCTRA_SAM ? 35 : 64);
			}
			else
			{
				num += 64;
			}
		}
	}
	else if (building->strength <= 1)
	{
		num = -1;
	}
	else if (building->strength < 128)
	{
		num = -2;
		if (!isra)
		{
			switch(building->type->ID)
			{
			case STRUCTTD_WEAP:
				num = 1;
				break;
			case STRUCTTD_REFINERY:
				num = 30;
				break;
			case STRUCTTD_EYE:
				num = 16;
				break;
			case STRUCTTD_STORAGE:
				num = 5;
				break;
			case STRUCTTD_REPAIR:
				num = 7;
				break;
			case STRUCTTD_PUMP:
				num = 14;
				break;
			}
		}
		else
		{
			switch (building->type->ID)
			{
			case STRUCTRA_WEAP:
			case STRUCTRA_FAKEWEAP:
				num = 1;
				break;
			case STRUCTRA_REFINERY:
				num = 30;
				break;
			case STRUCTRA_STORAGE:
				num = 5;
				break;
			case STRUCTRA_REPAIR:
				num = 7;
				break;
			case STRUCTRA_PUMP:
				num = 14;
				break;
			}
		}
	}
	Gdiplus::Rect buildingBounds;
	Tile* tile;
	Tile* factoryOverlayTile;
	TeamColor* bcolor;
	if (isra)
	{
		bcolor = TheTeamColorManagerRA->GetColor(building->house->StructColor.c_str());
	}
	else
	{
		bcolor = TheTeamColorManagerTD->GetColor(building->house->StructColor.c_str());
	}
	if (TheTilesetManager->GetTeamColorTileData(building->type->TileName.c_str(), num, bcolor, tile))
	{
		Gdiplus::Point location(topLeft.X * tileSize.Width, topLeft.Y * tileSize.Height);
		Gdiplus::Size size1(tile->Image->GetWidth() / tileScale, tile->Image->GetHeight() / tileScale);
		buildingBounds = Gdiplus::Rect(location, size1);
		factoryOverlayTile = nullptr;
		if (building->type->FactoryOverlay.length())
		{
			int shape = 0;
			if (building->strength < 128)
			{
				if (isra)
				{
					shape = 4;
				}
				else
				{
					shape = 10;
				}
			}
			TheTilesetManager->GetTeamColorTileData(building->type->FactoryOverlay.c_str(), shape, bcolor, factoryOverlayTile);
		}
		return std::make_pair(buildingBounds, [factoryOverlayTile, tile, building](Gdiplus::Graphics* graphics, Gdiplus::Rect buildingBounds)
		{
			Gdiplus::StringFormat stringFormat;
			stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
			Gdiplus::SolidBrush fakeBackgroundBrush = Gdiplus::SolidBrush(Gdiplus::Color(64, 0, 0, 0));
			Gdiplus::SolidBrush fakeTextBrush = Gdiplus::SolidBrush(Gdiplus::Color(128, 255, 255, 255));
			Gdiplus::SolidBrush baseBackgroundBrush = Gdiplus::SolidBrush(Gdiplus::Color(64, 0, 0, 0));
			Gdiplus::SolidBrush baseTextBrush = Gdiplus::SolidBrush(Gdiplus::Color(128, 255, 0, 0));
			if (factoryOverlayTile != nullptr)
			{
				graphics->DrawImage(tile->Image, buildingBounds, 0, 0, tile->Image->GetWidth(), tile->Image->GetHeight(), Gdiplus::UnitPixel);
				graphics->DrawImage(factoryOverlayTile->Image, buildingBounds, 0, 0, tile->Image->GetWidth(), tile->Image->GetHeight(), Gdiplus::UnitPixel);
			}
			else
			{
				graphics->DrawImage(tile->Image, buildingBounds, 0, 0, tile->Image->GetWidth(), tile->Image->GetHeight(), Gdiplus::UnitPixel);
			}
			if (building->type->IsFake)
			{
				const wchar_t* text = L"FAKE";
				NONCLIENTMETRICS cmetrics;
				cmetrics.cbSize = sizeof(cmetrics);
				SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(cmetrics), &cmetrics, 0);
				HDC dc = GetDC(nullptr);
				Gdiplus::Font font(dc, &cmetrics.lfCaptionFont);
				Gdiplus::SizeF size2;
				graphics->MeasureString(text, wcslen(text), &font, Gdiplus::SizeF(0, 0), nullptr, &size2);
				size2 = size2 + Gdiplus::SizeF(6, 6);
				Gdiplus::RectF rectangleF(Gdiplus::PointF(static_cast<float>(buildingBounds.X), static_cast<float>(buildingBounds.Y)), size2);
				graphics->FillRectangle(&fakeBackgroundBrush, rectangleF);
				graphics->DrawString(text, wcslen(text), &font, rectangleF, &stringFormat, &fakeTextBrush);
				ReleaseDC(nullptr, dc);
			}
			if (building->basePriority >= 0)
			{
				wchar_t buf[10];
				const wchar_t* text2 = _itow(building->basePriority, buf, 10);
				NONCLIENTMETRICS cmetrics;
				cmetrics.cbSize = sizeof(cmetrics);
				SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(cmetrics), &cmetrics, 0);
				HDC dc = GetDC(nullptr);
				Gdiplus::Font font(dc, &cmetrics.lfCaptionFont);
				Gdiplus::SizeF size3;
				graphics->MeasureString(text2, wcslen(text2), &font, Gdiplus::SizeF(0, 0), nullptr, &size3);
				size3 = size3 + Gdiplus::SizeF(6, 6);
				Gdiplus::Point p(buildingBounds.X, buildingBounds.Y);
				p = p + Gdiplus::Size(static_cast<int>((static_cast<float>(buildingBounds.Width) - size3.Width) / 2), static_cast<int>(static_cast<float>(buildingBounds.Height) - size3.Height));
				Gdiplus::RectF rectangleF2(Gdiplus::PointF(static_cast<float>(p.X), static_cast<float>(p.Y)), size3);
				graphics->FillRectangle(&baseBackgroundBrush, rectangleF2);
				graphics->DrawString(text2, wcslen(text2), &font, rectangleF2, &stringFormat, &baseTextBrush);
				ReleaseDC(nullptr, dc);
			}
		});
	}
	return std::make_pair(Gdiplus::Rect(), [](Gdiplus::Graphics*,Gdiplus::Rect) {});
}

typedef enum DirType : unsigned char {
	DIR_MIN = 0,
	DIR_N = 0,
	DIR_NE = 1 << 5,
	DIR_E = 2 << 5,
	DIR_SE = 3 << 5,
	DIR_S = 4 << 5,
	DIR_SW = 5 << 5,
	DIR_SW_X1 = (5 << 5) - 8,		// Direction of harvester while unloading.
	DIR_SW_X2 = (5 << 5) - 16,		// Direction of harvester while unloading.
	DIR_W = 6 << 5,
	DIR_NW = 7 << 5,
	DIR_MAX = 254
} DirType;

inline DirType operator + (DirType f1, DirType f2)
{
	return (DirType)(((int)f1 + (int)f2) & 0x00FF);
}

inline DirType operator + (DirType f1, int f2)
{
	return (DirType)(((int)f1 + (int)f2) & 0x00FF);
}

int __cdecl calcx(signed short param1, short distance)
{
	__asm {
		movzx	eax, [param1]
		mov	bx, [distance]
		imul 	bx
		shl	ax, 1
		rcl	dx, 1
		mov	al, ah
		mov	ah, dl
		cwd
	}
}

int __cdecl calcy(signed short param1, short distance)
{
	__asm {
		movzx	eax, [param1]
		mov	bx, [distance]
		imul bx
		shl	ax, 1
		rcl	dx, 1
		mov	al, ah
		mov	ah, dl
		cwd
		neg	eax
	}
}

void Move_Point(short& x, short& y, DirType dir, unsigned short distance)
{
	static unsigned char const CosTable[256] = {
		0x00,0x03,0x06,0x09,0x0c,0x0f,0x12,0x15,
		0x18,0x1b,0x1e,0x21,0x24,0x27,0x2a,0x2d,
		0x30,0x33,0x36,0x39,0x3b,0x3e,0x41,0x43,
		0x46,0x49,0x4b,0x4e,0x50,0x52,0x55,0x57,
		0x59,0x5b,0x5e,0x60,0x62,0x64,0x65,0x67,
		0x69,0x6b,0x6c,0x6e,0x6f,0x71,0x72,0x74,
		0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7b,
		0x7c,0x7d,0x7d,0x7e,0x7e,0x7e,0x7e,0x7e,
		0x7f,0x7e,0x7e,0x7e,0x7e,0x7e,0x7d,0x7d,
		0x7c,0x7b,0x7b,0x7a,0x79,0x78,0x77,0x76,
		0x75,0x74,0x72,0x71,0x70,0x6e,0x6c,0x6b,
		0x69,0x67,0x66,0x64,0x62,0x60,0x5e,0x5b,
		0x59,0x57,0x55,0x52,0x50,0x4e,0x4b,0x49,
		0x46,0x43,0x41,0x3e,0x3b,0x39,0x36,0x33,
		0x30,0x2d,0x2a,0x27,0x24,0x21,0x1e,0x1b,
		0x18,0x15,0x12,0x0f,0x0c,0x09,0x06,0x03,
		0x00,0xfd,0xfa,0xf7,0xf4,0xf1,0xee,0xeb,
		0xe8,0xe5,0xe2,0xdf,0xdc,0xd9,0xd6,0xd3,
		0xd0,0xcd,0xca,0xc7,0xc5,0xc2,0xbf,0xbd,
		0xba,0xb7,0xb5,0xb2,0xb0,0xae,0xab,0xa9,
		0xa7,0xa5,0xa2,0xa0,0x9e,0x9c,0x9a,0x99,
		0x97,0x95,0x94,0x92,0x91,0x8f,0x8e,0x8c,
		0x8b,0x8a,0x89,0x88,0x87,0x86,0x85,0x85,
		0x84,0x83,0x83,0x82,0x82,0x82,0x82,0x82,
		0x82,0x82,0x82,0x82,0x82,0x82,0x83,0x83,
		0x84,0x85,0x85,0x86,0x87,0x88,0x89,0x8a,
		0x8b,0x8c,0x8e,0x8f,0x90,0x92,0x94,0x95,
		0x97,0x99,0x9a,0x9c,0x9e,0xa0,0xa2,0xa5,
		0xa7,0xa9,0xab,0xae,0xb0,0xb2,0xb5,0xb7,
		0xba,0xbd,0xbf,0xc2,0xc5,0xc7,0xca,0xcd,
		0xd0,0xd3,0xd6,0xd9,0xdc,0xdf,0xe2,0xe5,
		0xe8,0xeb,0xee,0xf1,0xf4,0xf7,0xfa,0xfd,
	};
	static unsigned char const SinTable[256] = {
		0x7f,0x7e,0x7e,0x7e,0x7e,0x7e,0x7d,0x7d,
		0x7c,0x7b,0x7b,0x7a,0x79,0x78,0x77,0x76,
		0x75,0x74,0x72,0x71,0x70,0x6e,0x6c,0x6b,
		0x69,0x67,0x66,0x64,0x62,0x60,0x5e,0x5b,
		0x59,0x57,0x55,0x52,0x50,0x4e,0x4b,0x49,
		0x46,0x43,0x41,0x3e,0x3b,0x39,0x36,0x33,
		0x30,0x2d,0x2a,0x27,0x24,0x21,0x1e,0x1b,
		0x18,0x15,0x12,0x0f,0x0c,0x09,0x06,0x03,
		0x00,0xfd,0xfa,0xf7,0xf4,0xf1,0xee,0xeb,
		0xe8,0xe5,0xe2,0xdf,0xdc,0xd9,0xd6,0xd3,
		0xd0,0xcd,0xca,0xc7,0xc5,0xc2,0xbf,0xbd,
		0xba,0xb7,0xb5,0xb2,0xb0,0xae,0xab,0xa9,
		0xa7,0xa5,0xa2,0xa0,0x9e,0x9c,0x9a,0x99,
		0x97,0x95,0x94,0x92,0x91,0x8f,0x8e,0x8c,
		0x8b,0x8a,0x89,0x88,0x87,0x86,0x85,0x85,
		0x84,0x83,0x83,0x82,0x82,0x82,0x82,0x82,
		0x82,0x82,0x82,0x82,0x82,0x82,0x83,0x83,
		0x84,0x85,0x85,0x86,0x87,0x88,0x89,0x8a,
		0x8b,0x8c,0x8e,0x8f,0x90,0x92,0x94,0x95,
		0x97,0x99,0x9a,0x9c,0x9e,0xa0,0xa2,0xa5,
		0xa7,0xa9,0xab,0xae,0xb0,0xb2,0xb5,0xb7,
		0xba,0xbd,0xbf,0xc2,0xc5,0xc7,0xca,0xcd,
		0xd0,0xd3,0xd6,0xd9,0xdc,0xdf,0xe2,0xe5,
		0xe8,0xeb,0xee,0xf1,0xf4,0xf7,0xfa,0xfd,
		0x00,0x03,0x06,0x09,0x0c,0x0f,0x12,0x15,
		0x18,0x1b,0x1e,0x21,0x24,0x27,0x2a,0x2d,
		0x30,0x33,0x36,0x39,0x3b,0x3e,0x41,0x43,
		0x46,0x49,0x4b,0x4e,0x50,0x52,0x55,0x57,
		0x59,0x5b,0x5e,0x60,0x62,0x64,0x65,0x67,
		0x69,0x6b,0x6c,0x6e,0x6f,0x71,0x72,0x74,
		0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7b,
		0x7c,0x7d,0x7d,0x7e,0x7e,0x7e,0x7e,0x7e,
	};
	char* cos_table = (char*)&CosTable[0];
	x += static_cast<short>(calcx(cos_table[dir], distance));
	char* sin_table = (char*)&SinTable[0];
	y += static_cast<short>(calcy(sin_table[dir], distance));
}

void Normal_Move_Point(short& x, short& y, DirType dir, unsigned short distance)
{
	static unsigned char const CosTable[256] = {
		0x00,0x03,0x06,0x09,0x0c,0x0f,0x12,0x15,
		0x18,0x1b,0x1e,0x21,0x24,0x27,0x2a,0x2d,
		0x30,0x33,0x36,0x39,0x3b,0x3e,0x41,0x43,
		0x46,0x49,0x4b,0x4e,0x50,0x52,0x55,0x57,
		0x59,0x5b,0x5e,0x60,0x62,0x64,0x65,0x67,
		0x69,0x6b,0x6c,0x6e,0x6f,0x71,0x72,0x74,
		0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7b,
		0x7c,0x7d,0x7d,0x7e,0x7e,0x7e,0x7e,0x7e,
		0x7f,0x7e,0x7e,0x7e,0x7e,0x7e,0x7d,0x7d,
		0x7c,0x7b,0x7b,0x7a,0x79,0x78,0x77,0x76,
		0x75,0x74,0x72,0x71,0x70,0x6e,0x6c,0x6b,
		0x69,0x67,0x66,0x64,0x62,0x60,0x5e,0x5b,
		0x59,0x57,0x55,0x52,0x50,0x4e,0x4b,0x49,
		0x46,0x43,0x41,0x3e,0x3b,0x39,0x36,0x33,
		0x30,0x2d,0x2a,0x27,0x24,0x21,0x1e,0x1b,
		0x18,0x15,0x12,0x0f,0x0c,0x09,0x06,0x03,
		0x00,0xfd,0xfa,0xf7,0xf4,0xf1,0xee,0xeb,
		0xe8,0xe5,0xe2,0xdf,0xdc,0xd9,0xd6,0xd3,
		0xd0,0xcd,0xca,0xc7,0xc5,0xc2,0xbf,0xbd,
		0xba,0xb7,0xb5,0xb2,0xb0,0xae,0xab,0xa9,
		0xa7,0xa5,0xa2,0xa0,0x9e,0x9c,0x9a,0x99,
		0x97,0x95,0x94,0x92,0x91,0x8f,0x8e,0x8c,
		0x8b,0x8a,0x89,0x88,0x87,0x86,0x85,0x85,
		0x84,0x83,0x83,0x82,0x82,0x82,0x82,0x82,
		0x82,0x82,0x82,0x82,0x82,0x82,0x83,0x83,
		0x84,0x85,0x85,0x86,0x87,0x88,0x89,0x8a,
		0x8b,0x8c,0x8e,0x8f,0x90,0x92,0x94,0x95,
		0x97,0x99,0x9a,0x9c,0x9e,0xa0,0xa2,0xa5,
		0xa7,0xa9,0xab,0xae,0xb0,0xb2,0xb5,0xb7,
		0xba,0xbd,0xbf,0xc2,0xc5,0xc7,0xca,0xcd,
		0xd0,0xd3,0xd6,0xd9,0xdc,0xdf,0xe2,0xe5,
		0xe8,0xeb,0xee,0xf1,0xf4,0xf7,0xfa,0xfd,
	};
	static unsigned char const SinTable[256] = {
		0x7f,0x7e,0x7e,0x7e,0x7e,0x7e,0x7d,0x7d,
		0x7c,0x7b,0x7b,0x7a,0x79,0x78,0x77,0x76,
		0x75,0x74,0x72,0x71,0x70,0x6e,0x6c,0x6b,
		0x69,0x67,0x66,0x64,0x62,0x60,0x5e,0x5b,
		0x59,0x57,0x55,0x52,0x50,0x4e,0x4b,0x49,
		0x46,0x43,0x41,0x3e,0x3b,0x39,0x36,0x33,
		0x30,0x2d,0x2a,0x27,0x24,0x21,0x1e,0x1b,
		0x18,0x15,0x12,0x0f,0x0c,0x09,0x06,0x03,
		0x00,0xfd,0xfa,0xf7,0xf4,0xf1,0xee,0xeb,
		0xe8,0xe5,0xe2,0xdf,0xdc,0xd9,0xd6,0xd3,
		0xd0,0xcd,0xca,0xc7,0xc5,0xc2,0xbf,0xbd,
		0xba,0xb7,0xb5,0xb2,0xb0,0xae,0xab,0xa9,
		0xa7,0xa5,0xa2,0xa0,0x9e,0x9c,0x9a,0x99,
		0x97,0x95,0x94,0x92,0x91,0x8f,0x8e,0x8c,
		0x8b,0x8a,0x89,0x88,0x87,0x86,0x85,0x85,
		0x84,0x83,0x83,0x82,0x82,0x82,0x82,0x82,
		0x82,0x82,0x82,0x82,0x82,0x82,0x83,0x83,
		0x84,0x85,0x85,0x86,0x87,0x88,0x89,0x8a,
		0x8b,0x8c,0x8e,0x8f,0x90,0x92,0x94,0x95,
		0x97,0x99,0x9a,0x9c,0x9e,0xa0,0xa2,0xa5,
		0xa7,0xa9,0xab,0xae,0xb0,0xb2,0xb5,0xb7,
		0xba,0xbd,0xbf,0xc2,0xc5,0xc7,0xca,0xcd,
		0xd0,0xd3,0xd6,0xd9,0xdc,0xdf,0xe2,0xe5,
		0xe8,0xeb,0xee,0xf1,0xf4,0xf7,0xfa,0xfd,
		0x00,0x03,0x06,0x09,0x0c,0x0f,0x12,0x15,
		0x18,0x1b,0x1e,0x21,0x24,0x27,0x2a,0x2d,
		0x30,0x33,0x36,0x39,0x3b,0x3e,0x41,0x43,
		0x46,0x49,0x4b,0x4e,0x50,0x52,0x55,0x57,
		0x59,0x5b,0x5e,0x60,0x62,0x64,0x65,0x67,
		0x69,0x6b,0x6c,0x6e,0x6f,0x71,0x72,0x74,
		0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7b,
		0x7c,0x7d,0x7d,0x7e,0x7e,0x7e,0x7e,0x7e,
	};
	static const char* _sin_table = (char*)&SinTable[0];
	static const char* _cos_table = (char*)&CosTable[0];
	x += static_cast<short>(calcx(_cos_table[dir], distance));
	y += static_cast<short>(calcy(_sin_table[dir] / 2, distance));
}

FacingType Dir_Facing(DirType facing) { return (FacingType)(((unsigned char)(facing + 0x10) & 0xFF) >> 5); }

void Vessel_Turret_Adjust(int id, DirType dir, int& x, int& y)
{
	short xx = static_cast<short>(x);
	short yy = static_cast<short>(y);

	switch (id) {
	case VESSELRA_CA:
		Normal_Move_Point(xx, yy, dir, 22);
		x = xx;
		y = yy - 4;
		break;

	case VESSELRA_PT:
		Normal_Move_Point(xx, yy, dir, 14);
		x = xx;
		y = yy + 1;
		break;

	case VESSELRA_DD:
		Normal_Move_Point(xx, yy, dir + DIR_S, 8);
		x = xx;
		y = yy - 4;
		break;
	}
}

std::pair<Gdiplus::Rect, RenderFunc> MapRender::Render(bool isra, Gdiplus::Point topLeft, Gdiplus::Size tileSize, Aircraft* unit)
{
	int icon = BodyShape[Facing32[unit->direction.ID]];
	if (isra)
	{
		if (unit->type->ID != AIRCRAFTRA_LONGBOW && unit->type->ID != AIRCRAFTRA_HIND && unit->type->ID != AIRCRAFTRA_TRANSPORT)
		{
			icon = BodyShape[Facing16[unit->direction.ID] * 2] / 2;
		}
	}
	Gdiplus::Rect renderBounds;
	Tile* tile = nullptr;
	Tile* rrotortile = nullptr;
	Tile* lrotortile = nullptr;
	TeamColor* ucolor;
	if (isra)
	{
		ucolor = TheTeamColorManagerRA->GetColor(unit->house->UnitColor.c_str());
	}
	else
	{
		ucolor = TheTeamColorManagerTD->GetColor(unit->house->UnitColor.c_str());
	}
	if (TheTilesetManager->GetTeamColorTileData(unit->type->Name.c_str(), icon, ucolor, tile))
	{
		if (unit->type->RotorCount)
		{
			TheTilesetManager->GetTileData("RROTOR", 4, rrotortile);
		}
		if (unit->type->RotorCount == 2)
		{
			TheTilesetManager->GetTileData("LROTOR", 4, lrotortile);
		}
		Gdiplus::Point pt = Gdiplus::Point(topLeft.X * tileSize.Width, topLeft.Y * tileSize.Height) + Gdiplus::Size(tileSize.Width / 2, tileSize.Height / 2);
		renderBounds = Gdiplus::Rect(pt - Gdiplus::Size(unit->type->RenderSize.Width / 2, unit->type->RenderSize.Height / 2), unit->type->RenderSize);
		return std::make_pair(renderBounds, [tile, rrotortile, lrotortile, unit, tileSize, topLeft](Gdiplus::Graphics* graphics, Gdiplus::Rect renderBounds)
			{
				graphics->DrawImage(tile->Image, renderBounds, 0, 0, tile->Image->GetWidth(), tile->Image->GetHeight(), Gdiplus::UnitPixel);
				if (lrotortile)
				{
					Gdiplus::Point pt(renderBounds.X, renderBounds.Y);
					pt = pt + Gdiplus::Size(renderBounds.Width / 2, renderBounds.Height / 2);
					short x = 0;
					short y = 0;
					DirType SecondaryFacing = static_cast<DirType>(unit->direction.ID);
					FacingType face = Dir_Facing(SecondaryFacing);
					Move_Point(x, y, SecondaryFacing, _stretch[face]);
					pt.X += x * tileSize.Width / 24;
					pt.Y += (y - 2) * tileSize.Width / 24;
					Gdiplus::Size rrotorSize(rrotortile->Image->GetWidth() / TileScale, rrotortile->Image->GetHeight() / TileScale);
					Gdiplus::Rect rrotorBounds(pt - Gdiplus::Size(rrotorSize.Width / 2, rrotorSize.Height / 2), rrotorSize);
					graphics->DrawImage(rrotortile->Image, rrotorBounds, 0, 0, rrotortile->Image->GetWidth(), rrotortile->Image->GetHeight(), Gdiplus::UnitPixel);
					pt.X -= x * tileSize.Width / 24;
					pt.Y -= (y - 2) * tileSize.Width / 24;
					Move_Point(x, y, SecondaryFacing + DIR_S, _stretch[face] * 2);
					pt.X += x * tileSize.Width / 24;
					pt.Y += (y - 2) * tileSize.Width / 24;
					Gdiplus::Size lrotorSize(lrotortile->Image->GetWidth() / TileScale, lrotortile->Image->GetHeight() / TileScale);
					Gdiplus::Rect lrotorBounds(pt - Gdiplus::Size(lrotorSize.Width / 2, lrotorSize.Height / 2), lrotorSize);
					graphics->DrawImage(lrotortile->Image, lrotorBounds, 0, 0, lrotortile->Image->GetWidth(), lrotortile->Image->GetHeight(), Gdiplus::UnitPixel);
				}
				else if (rrotortile)
				{
					Gdiplus::Point pt(renderBounds.X, renderBounds.Y);
					pt = pt + Gdiplus::Size(renderBounds.Width / 2, renderBounds.Height / 2);
					pt.Y -= 2;
					Gdiplus::Size rrotorSize(rrotortile->Image->GetWidth() / TileScale, rrotortile->Image->GetHeight() / TileScale);
					Gdiplus::Rect rrotorBounds(pt - Gdiplus::Size(rrotorSize.Width / 2, rrotorSize.Height / 2), rrotorSize);
					graphics->DrawImage(rrotortile->Image, rrotorBounds, 0, 0, rrotortile->Image->GetWidth(), rrotortile->Image->GetHeight(), Gdiplus::UnitPixel);
				}
			});
	}
	return std::make_pair(Gdiplus::Rect(), [](Gdiplus::Graphics*, Gdiplus::Rect) {});
}

std::pair<Gdiplus::Rect, RenderFunc> MapRender::Render(bool isra, Gdiplus::Point topLeft, Gdiplus::Size tileSize, Vessel* unit)
{
	int icon = BodyShape[Facing16[unit->direction.ID] * 2] >> 1;
	if (isra)
	{
		if (unit->type->ID == VESSELRA_TRANSPORT || unit->type->ID == VESSELRA_CARRIER)
		{
			icon = 0;
		}
	}
	Gdiplus::Rect renderBounds;
	Tile* tile = nullptr;
	Tile* turrettile = nullptr;
	TeamColor* ucolor;
	if (isra)
	{
		ucolor = TheTeamColorManagerRA->GetColor(unit->house->UnitColor.c_str());
	}
	else
	{
		ucolor = TheTeamColorManagerTD->GetColor(unit->house->UnitColor.c_str());
	}
	if (TheTilesetManager->GetTeamColorTileData(unit->type->Name.c_str(), icon, ucolor, tile))
	{
		int facing = Facing32[unit->direction.ID];
		int tfacing = facing;
		if (unit->type->HasTurret)
		{
			int num2 = BodyShape[tfacing];
			const char* shape = nullptr;
			switch (unit->type->ID)
			{
			case VESSELRA_CA:
				shape = "TURR";
				break;
			case VESSELRA_DD:
				shape = "SSAM";
				break;
			case VESSELRA_PT:
				shape = "MGUN";
				break;
			}
			if (shape)
			{
				TheTilesetManager->GetTileData(shape, num2, turrettile);
			}
		}
		Gdiplus::Point pt = Gdiplus::Point(topLeft.X * tileSize.Width, topLeft.Y * tileSize.Height) + Gdiplus::Size(tileSize.Width / 2, tileSize.Height / 2);
		renderBounds = Gdiplus::Rect(pt - Gdiplus::Size(unit->type->RenderSize.Width / 2, unit->type->RenderSize.Height / 2), unit->type->RenderSize);
		return std::make_pair(renderBounds, [tile, unit, turrettile, tileSize, topLeft](Gdiplus::Graphics* graphics, Gdiplus::Rect renderBounds)
			{
				graphics->DrawImage(tile->Image, renderBounds, 0, 0, tile->Image->GetWidth(), tile->Image->GetHeight(), Gdiplus::UnitPixel);
				if (turrettile)
				{
					Gdiplus::Point pt(renderBounds.X, renderBounds.Y);
					pt = pt + Gdiplus::Size(renderBounds.Width / 2, renderBounds.Height / 2);
					DirType turdir = static_cast<DirType>(Facing16[unit->direction.ID] * 16);
					int x = 0;
					int y = 0;
					if (unit->type->ID == VESSELRA_CA)
					{
						Vessel_Turret_Adjust(unit->type->ID, turdir, x, y);
						pt.X += x * tileSize.Width / 24;
						pt.Y += y * tileSize.Width / 24;
						Gdiplus::Size turret2Size(turrettile->Image->GetWidth() / TileScale, turrettile->Image->GetHeight() / TileScale);
						Gdiplus::Rect turret2Bounds(pt - Gdiplus::Size(turret2Size.Width / 2, turret2Size.Height / 2), turret2Size);
						graphics->DrawImage(turrettile->Image, turret2Bounds, 0, 0, turrettile->Image->GetWidth(), turrettile->Image->GetHeight(), Gdiplus::UnitPixel);
						pt.X -= x * tileSize.Width / 24;
						pt.Y -= y * tileSize.Width / 24;
						x = 0;
						y = 0;
						DirType PrimaryFacing = static_cast<DirType>(unit->direction.ID);
						turdir = static_cast<DirType>(Facing16[PrimaryFacing+DIR_S] * 16);
					}
					Vessel_Turret_Adjust(unit->type->ID, turdir, x, y);
					pt.X += x * tileSize.Width / 24;
					pt.Y += y * tileSize.Width / 24;
					Gdiplus::Size turretSize(turrettile->Image->GetWidth() / TileScale, turrettile->Image->GetHeight() / TileScale);
					Gdiplus::Rect turretBounds(pt - Gdiplus::Size(turretSize.Width / 2, turretSize.Height / 2), turretSize);
					graphics->DrawImage(turrettile->Image, turretBounds, 0, 0, turrettile->Image->GetWidth(), turrettile->Image->GetHeight(), Gdiplus::UnitPixel);
				}
			});
	}
	return std::make_pair(Gdiplus::Rect(), [](Gdiplus::Graphics*, Gdiplus::Rect) {});
}
