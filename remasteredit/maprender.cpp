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
#include "boolinq.h"
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
		for (auto i : map->technos->GetOccupiers(typeid(Terrain)))
		{
			Terrain* o = dynamic_cast<Terrain*>(i.first);
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
					int twidth = tile->Image->GetWidth() / TileWidth;
					int theight = tile->Image->GetHeight() / TileHeight;
					int xpos = p.X * TileWidth;
					int ypos = p.Y * TileHeight;
					Gdiplus::Rect r;
					r.X = xpos;
					r.Y = ypos;
					r.Width = twidth;
					r.Height = theight;
					graphics->DrawImage(tile->Image, r);
				}
			}
		}
	}
	if ((layers & MAPLAYER_BUILDINGS) != 0)
	{
		for (auto i : map->technos->GetOccupiers(typeid(Structure)))
		{
			Structure* building = dynamic_cast<Structure*>(i.first);
			Gdiplus::Point topLeft = i.second;
			if (locations.empty() || locations.count(topLeft))
			{
				list.push_back(Render(map->isra, topLeft, tileSize, tileScale, building));
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
				num += (!_stricmp(building->type->Name.c_str(), "sam") ? 35 : 64);
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
		if (!_stricmp(building->type->Name.c_str(), "weap") || !_stricmp(building->type->Name.c_str(), "weaf"))
		{
			num = 1;
		}
		else if (!isra && !_stricmp(building->type->Name.c_str(), "proc"))
		{
			num = 30;
		}
		else if (!_stricmp(building->type->Name.c_str(), "eye"))
		{
			num = 16;
		}
		else if (!_stricmp(building->type->Name.c_str(), "silo"))
		{
			num = 5;
		}
		else if (!_stricmp(building->type->Name.c_str(), "fix"))
		{
			num = 7;
		}
		else if (!_stricmp(building->type->Name.c_str(), "v19"))
		{
			num = 14;
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
