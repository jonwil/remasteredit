#include "map.h"
#include "tileset.h"
#include "TemplateType.h"
#include "overlaytype.h"
#include "smudgetype.h"
#include "TerrainType.h"
#include "INI.H"
#include "RAWFILE.H"
#include "XSTRAW.H"
#include "LCWSTRAW.H"
#include <Shlwapi.h>
#include <time.h>
char _staging_buffer[32000];
std::map<std::string, const char**> TDTheaters;
std::map<std::string, const char**> RATheaters;
const char* TDDesert[5] = { "TD_Units", "TD_Structures", "TD_VFX", "Common_VFX", "TD_Terrain_Desert" };
const char* TDTemperate[5] = { "TD_Units", "TD_Structures", "TD_VFX", "Common_VFX", "TD_Terrain_Temperate" };
const char* TDWinter[5] = { "TD_Units", "TD_Structures", "TD_VFX", "Common_VFX", "TD_Terrain_Winter" };
const char* RATemperate[5] = { "RA_Units", "RA_Structures", "RA_VFX", "Common_VFX", "RA_Terrain_Temperate" };
const char* RASnow[5] = { "RA_Units", "RA_Structures", "RA_VFX", "Common_VFX", "RA_Terrain_Snow" };
const char* RAInterior[5] = { "RA_Units", "RA_Structures", "RA_VFX", "Common_VFX", "RA_Terrain_Interior" };
#define	RA_MAP_CELL_W				128
#define	RA_MAP_CELL_H				128
#define	RA_MAP_CELL_TOTAL			(RA_MAP_CELL_W*RA_MAP_CELL_H)
#define	TD_MAP_CELL_MAX_X_BITS		6
#define	TD_MAP_CELL_MAX_Y_BITS		6
#define	TD_MAP_CELL_W				(1<<TD_MAP_CELL_MAX_X_BITS)
#define	TD_MAP_CELL_H				(1<<TD_MAP_CELL_MAX_Y_BITS)
#define	TD_MAP_CELL_TOTAL			(TD_MAP_CELL_W*TD_MAP_CELL_H)
std::map<std::string, int> TDTheaterIDs;
std::map<std::string, int> RATheaterIDs;
int tilescale = 2;
int tilesize = 128 / tilescale;
Map::Map() : width(0), height(0), size(0), templates(nullptr), overlays(nullptr), smudges(nullptr), terrains(nullptr), isra(false), theaterid(0)
{
	tilesize = 128 / tilescale;
	TDTheaters["desert"] = TDDesert;
	TDTheaters["temperate"] = TDTemperate;
	TDTheaters["winter"] = TDWinter;
	RATheaters["temperate"] = RATemperate;
	RATheaters["snow"] = RASnow;
	RATheaters["interior"] = RAInterior;
	TDTheaterIDs["desert"] = THEATERF_DESERTTD;
	TDTheaterIDs["temperate"] = THEATERF_TEMPERATETD;
	TDTheaterIDs["winter"] = THEATERF_WINTERTD;
	RATheaterIDs["temperate"] = THEATERF_TEMPERATERA;
	RATheaterIDs["snow"] = THEATERF_SNOWRA;
	RATheaterIDs["interior"] = THEATERF_INTERIORRA;
}

Map::~Map()
{
	delete[] templates;
	delete[] overlays;
	delete[] smudges;
	delete[] terrains;
}

bool Map::GetAdjacentPoint(int x, int y, FacingType facing, int &outx, int &outy)
{
	int adjx = x;
	int adjy = y;
	switch (facing)
	{
	case FACING_NORTH:
		adjy--;
		break;
	case FACING_NORTHEAST:
		adjx++;
		adjy--;
		break;
	case FACING_EAST:
		adjx++;
		break;
	case FACING_SOUTHEAST:
		adjx++;
		adjy++;
		break;
	case FACING_SOUTH:
		adjy++;
		break;
	case FACING_SOUTHWEST:
		adjx--;
		adjy++;
		break;
	case FACING_WEST:
		adjx--;
		break;
	case FACING_NORTHWEST:
		adjx--;
		adjy--;
		break;
	}
	outx = adjx;
	outy = adjy;
	if (outx < 0 || outx > width)
	{
		return false;
	}
	if (outy < 0 || outy > height)
	{
		return false;
	}
	return true;
}

Overlay* Map::GetAdjacentOverlay(int x, int y, FacingType facing)
{
	int px;
	int py;
	if (GetAdjacentPoint(x, y, facing, px, py))
	{
		int cell = (py * width) + px;
		return &overlays[cell];
	}
	return nullptr;
}

void Map::Load(const char* path)
{
	srand((unsigned int)time(nullptr) + GetCurrentProcessId());
	RawFileClass f(path);
	f.Open();
	INIClass ini;
	ini.Load(f);
	char bpath[MAX_PATH];
	strcpy(bpath, __argv[2]);
	PathRenameExtension(bpath, ".bin");
	RawFileClass bin(bpath);
	isra = true;
	if (bin.Is_Available())
	{
		isra = false;
	}
	if (isra)
	{
		char th[9];
		ini.Get_String("Map", "Theater", "", th, 9);
		theater = th;
		theaterid = RATheaterIDs[theater];
		TheTilesetManager->SetSets(RATheaters[theater]);
		width = RA_MAP_CELL_W;
		height = RA_MAP_CELL_H;
		size = RA_MAP_CELL_TOTAL;
		if (templates)
		{
			delete[] templates;
		}
		templates = new Template[RA_MAP_CELL_TOTAL];
		memset(templates, 0, RA_MAP_CELL_TOTAL * sizeof(Template));
		int clen = ini.Get_UUBlock("MapPack", _staging_buffer, sizeof(_staging_buffer));
		BufferStraw cbstraw(_staging_buffer, clen);
		LCWStraw cdecomp(LCWStraw::DECOMPRESS);
		cdecomp.Get_From(&cbstraw);
		for (int i = 0; i < RA_MAP_CELL_TOTAL; i++)
		{
			unsigned short s;
			cdecomp.Get(&s, sizeof(s));
			const TemplateType* t = nullptr;
			if (s < TEMPLATERA_COUNT)
			{
				t = TemplateType::PointersRA[s];
			}
			if (t != nullptr)
			{
				if (t->Theater & theaterid)
				{
					templates[i].type = t;
				}
			}
		}
		for (int i = 0; i < RA_MAP_CELL_TOTAL; i++)
		{
			unsigned char b;
			cdecomp.Get(&b, sizeof(b));
			if (templates[i].type)
			{
				if (templates[i].type->ID != TEMPLATERA_CLEAR1 && b >= templates[i].type->IconWidth * templates[i].type->IconHeight)
				{
					templates[i].type = nullptr;
				}
				else
				{
					templates[i].icon = b;
				}
			}
		}
		if (overlays)
		{
			delete[] overlays;
		}
		overlays = new Overlay[RA_MAP_CELL_TOTAL];
		memset(overlays, 0, RA_MAP_CELL_TOTAL * sizeof(Overlay));
		int olen = ini.Get_UUBlock("OverlayPack", _staging_buffer, sizeof(_staging_buffer));
		BufferStraw obstraw(_staging_buffer, olen);
		LCWStraw odecomp(LCWStraw::DECOMPRESS);
		odecomp.Get_From(&obstraw);
		for (int i = 0; i < RA_MAP_CELL_TOTAL; i++)
		{
			char c;
			cdecomp.Get(&c, sizeof(c));
			const OverlayType* o = nullptr;
			if (c < OVERLAYRA_COUNT)
			{
				o = OverlayType::PointersRA[c];
			}
			if (o != nullptr)
			{
				if (o->Theater & theaterid)
				{
					overlays[i].type = o;
				}
			}
		}
		if (smudges)
		{
			delete[] smudges;
		}
		smudges = new Smudge[RA_MAP_CELL_TOTAL];
		memset(smudges, 0, RA_MAP_CELL_TOTAL * sizeof(Smudge));
		for (int i = 0; i < ini.Entry_Count("Smudge"); i++)
		{
			const char* cell = ini.Get_Entry("Smudge", i);
			char smudge[100];
			ini.Get_String("Smudge", cell, nullptr, smudge, 100);
			char* name = strtok(smudge, ",");
			int cellid = atoi(strtok(nullptr, ","));
			int data = atoi(strtok(nullptr, ","));
			if (SmudgeType::SmudgeMapTD.count(name))
			{
				smudges[cellid].type = SmudgeType::SmudgeMapTD[name];
				smudges[cellid].data = data;
			}
		}
		if (terrains)
		{
			delete[] terrains;
		}
		terrains = new Terrain[RA_MAP_CELL_TOTAL];
		memset(terrains, 0, RA_MAP_CELL_TOTAL * sizeof(Terrain));
		for (int i = 0; i < ini.Entry_Count("Terrain"); i++)
		{
			const char* cell = ini.Get_Entry("Terrain", i);
			int cellid = atoi(cell);
			char terrain[100];
			ini.Get_String("Terrain", cell, nullptr, terrain, 100);
			if (TerrainType::TerrainMapTD.count(terrain))
			{
				terrains[cellid].type = TerrainType::TerrainMapTD[terrain];
				terrains[cellid].Trigger = _strdup("");
			}
		}
	}
	else
	{
		char th[9];
		ini.Get_String("Map", "Theater", "", th, 9);
		theater = th;
		theaterid = TDTheaterIDs[theater];
		TheTilesetManager->SetSets(TDTheaters[theater]);
		bin.Open();
		width = TD_MAP_CELL_W;
		height = TD_MAP_CELL_H;
		size = TD_MAP_CELL_TOTAL;
		if (templates)
		{
			delete[] templates;
		}
		templates = new Template[TD_MAP_CELL_TOTAL];
		memset(templates, 0, TD_MAP_CELL_TOTAL * sizeof(Template));
		for (int i = 0; i < TD_MAP_CELL_TOTAL; i++)
		{
			unsigned char tile;
			unsigned char icon;
			bin.Read(&tile, sizeof(tile));
			bin.Read(&icon, sizeof(icon));
			const TemplateType* t = nullptr;
			if (tile < TEMPLATETD_COUNT)
			{
				t = TemplateType::PointersTD[tile];
			}
			if (t != nullptr)
			{
				if (t->Theater & theaterid)
				{
					templates[i].type = t;
					templates[i].icon = icon;
				}
			}
		}
		if (overlays)
		{
			delete[] overlays;
		}
		overlays = new Overlay[TD_MAP_CELL_TOTAL];
		memset(overlays, 0, TD_MAP_CELL_TOTAL * sizeof(Overlay));
		for (int i = 0; i < ini.Entry_Count("Overlay"); i++)
		{
			const char* cell = ini.Get_Entry("Overlay", i);
			int cellid = atoi(cell);
			char overlay[9];
			ini.Get_String("Overlay", cell, nullptr, overlay, 9);
			if (OverlayType::OverlayMapTD.count(overlay))
			{
				overlays[cellid].type = OverlayType::OverlayMapTD[overlay];
			}
		}
		if (smudges)
		{
			delete[] smudges;
		}
		smudges = new Smudge[RA_MAP_CELL_TOTAL];
		memset(smudges, 0, RA_MAP_CELL_TOTAL * sizeof(Smudge));
		for (int i = 0; i < ini.Entry_Count("Smudge"); i++)
		{
			const char* cell = ini.Get_Entry("Smudge", i);
			char smudge[100];
			ini.Get_String("Smudge", cell, nullptr, smudge, 100);
			char* name = strtok(smudge, ",");
			int cellid = atoi(strtok(nullptr, ","));
			int data = atoi(strtok(nullptr, ","));
			if (SmudgeType::SmudgeMapTD.count(name))
			{
				smudges[cellid].type = SmudgeType::SmudgeMapTD[name];
				smudges[cellid].data = data;
			}
		}
		if (terrains)
		{
			delete[] terrains;
		}
		terrains = new Terrain[TD_MAP_CELL_TOTAL];
		memset(terrains, 0, TD_MAP_CELL_TOTAL * sizeof(Terrain));
		for (int i = 0; i < ini.Entry_Count("Terrain"); i++)
		{
			const char* cell = ini.Get_Entry("Terrain", i);
			int cellid = atoi(cell);
			char terrain[100];
			ini.Get_String("Terrain", cell, nullptr, terrain, 100);
			char* name = strtok(terrain, ",");
			char* trigger = strtok(nullptr, ",");
			if (TerrainType::TerrainMapTD.count(name))
			{
				terrains[cellid].type = TerrainType::TerrainMapTD[name];
				terrains[cellid].Trigger = _strdup(trigger);
				if (terrains[cellid].type->IsTransformable)
				{
					terrains[cellid].icon = 22;
				}
			}
		}
	}
	static int icons1[9] = { 0, 1, 3, 4, 6, 7, 8, 10, 11 };
	static int icons2[9] = { 0, 0, 0, 1, 1, 1, 2, 2, 2 };
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int cell = (y * width) + x;
			Overlay* o = &overlays[cell];
			if (o->type)
			{
				if (o->type->Flag == OVERLAYTYPE_TIBERIUMORGOLD || o->type->Flag == OVERLAYTYPE_GEMS)
				{
					int count = 0;
					for (int i = 0; i < FACING_COUNT; i++)
					{
						Overlay* ao = GetAdjacentOverlay(x, y, (FacingType)i);
						if (ao && ao->type && (ao->type->Flag == OVERLAYTYPE_TIBERIUMORGOLD || ao->type->Flag == OVERLAYTYPE_GEMS))
						{
							count++;
						}
					}
					if (o->type->Flag == OVERLAYTYPE_TIBERIUMORGOLD)
					{
						o->icon = icons1[count];
					}
					else
					{
						o->icon = icons2[count];
					}
				}
				if (o->type->Flag == OVERLAYTYPE_WALL)
				{
					Overlay* north = GetAdjacentOverlay(x, y, FACING_NORTH);
					Overlay* east = GetAdjacentOverlay(x, y, FACING_EAST);
					Overlay* south = GetAdjacentOverlay(x, y, FACING_SOUTH);
					Overlay* west = GetAdjacentOverlay(x, y, FACING_WEST);
					int count = 0;
					if (north && north->type == o->type)
					{
						count |= 1;
					}
					if (east && east->type == o->type)
					{
						count |= 2;
					}
					if (south && south->type == o->type)
					{
						count |= 4;
					}
					if (west && west->type == o->type)
					{
						count |= 8;
					}
					o->icon = count;
				}
			}
		}
	}
}

void Map::Render(Gdiplus::Graphics *graphics)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int cell = (i * width) + j;
			Template* t = &templates[cell];
			const char* name = "clear1";
			int icon = ((cell & 3) | ((cell >> 4) & 12));
			if (t->type)
			{
				name = t->type->Name.c_str();
				icon = t->icon;
			}
			Tile* tile = TheTilesetManager->GetTileImage(name, icon, 0);
			if (tile)
			{
				Gdiplus::Rect r;
				r.X = j * tilesize;
				r.Y = i * tilesize;
				r.Width = tilesize;
				r.Height = tilesize;
				graphics->DrawImage(tile->Image, r);
			}
		}
	}
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int cell = (i * width) + j;
			Smudge* o = &smudges[cell];
			const char* name = nullptr;
			int icon = 0;
			if (o->type)
			{
				name = o->type->Name.c_str();
				icon = o->icon;
			}
			if (name)
			{
				Tile* tile = TheTilesetManager->GetTileImage(name, icon, 0);
				if (tile)
				{
					int twidth = tile->Image->GetWidth() / tilescale;
					int theight = tile->Image->GetHeight() / tilescale;
					int xpos = j * tilesize;
					int ypos = i * tilesize;
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
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int cell = (i * width) + j;
			Overlay* o = &overlays[cell];
			const char* name = nullptr;
			int icon = 0;
			if (o->type)
			{
				name = o->type->Name.c_str();
				if (o->type->Flag == OVERLAYTYPE_TIBERIUMORGOLD && !isra)
				{
					name = OverlayType::Tiberium[rand() % OverlayType::Tiberium.size()]->Name.c_str();
				}
				if (o->type->Flag == OVERLAYTYPE_TIBERIUMORGOLD && isra)
				{
					name = OverlayType::Gold[rand() % OverlayType::Gold.size()]->Name.c_str();
				}
				if (o->type->Flag == OVERLAYTYPE_GEMS)
				{
					name = OverlayType::Gems[rand() % OverlayType::Gems.size()]->Name.c_str();
				}
				icon = o->icon;
			}
			if (name)
			{
				Tile* tile = TheTilesetManager->GetTileImage(name, icon, 0);
				if (tile)
				{
					int twidth = tilesize;
					int theight = tilesize;
					if (o->type->Flag == OVERLAYTYPE_CRATE || o->type->Flag == OVERLAYTYPE_FLAG)
					{
						twidth = tile->Image->GetWidth() / tilescale;
						theight = tile->Image->GetHeight() / tilescale;
					}
					int xpos = (j * tilesize) + (tilesize / 2) - (twidth / 2);
					int ypos = (i * tilesize) + (tilesize / 2) - (theight / 2);
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
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int cell = (i * width) + j;
			Terrain* o = &terrains[cell];
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
				Tile* tile = TheTilesetManager->GetTileImage(name, icon, 0);
				if (tile)
				{
					int twidth = tile->Image->GetWidth() / tilescale;
					int theight = tile->Image->GetHeight() / tilescale;
					int xpos = j * tilesize;
					int ypos = i * tilesize;
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
}
