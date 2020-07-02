#include "cellmetrics.h"
#include "map.h"
#include "tileset.h"
#include "TemplateType.h"
#include "overlaytype.h"
#include "smudgetype.h"
#include "TerrainType.h"
#include "structtype.h"
#include "INI.H"
#include "RAWFILE.H"
#include "XSTRAW.H"
#include "LCWSTRAW.H"
#include <Shlwapi.h>
#include <time.h>
#include "boolinq.h"
#include "housetype.h"
#include "teamcolor.h"
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
static int tilescale = 2;
static int tilesize = 128 / tilescale;
const FacingType CellMetrics::AdjacentFacings[8] = { FACING_NORTH, FACING_NORTHEAST, FACING_EAST, FACING_SOUTHEAST, FACING_SOUTH, FACING_SOUTHWEST, FACING_WEST, FACING_NORTHWEST };
DirectionType DirectionType::Types[16] = {
	{0, "North", FACING_NORTH},
	{16, "North-NorthEast", FACING_NONE},
	{32, "NorthEast", FACING_NORTHEAST},
	{48, "East-NorthEast", FACING_NONE},
	{64, "East", FACING_EAST},
	{80, "East-SouthEast", FACING_NONE},
	{96, "SouthEast", FACING_SOUTHEAST},
	{112, "South-SouthEast", FACING_NONE},
	{128, "South", FACING_SOUTH},
	{144, "South-SouthWest", FACING_NONE},
	{160, "SouthWest", FACING_SOUTHWEST},
	{176, "West-SouthWest", FACING_NONE},
	{192, "West", FACING_WEST},
	{208, "West-NorthWest", FACING_NONE},
	{224, "NorthWest", FACING_NORTHWEST},
	{240, "North-NorthWest", FACING_NONE}
};
Map::Map() : width(0), height(0), size(0), templates(nullptr), overlays(nullptr), smudges(nullptr), technos(nullptr), buildings(nullptr), isra(false), theaterid(0), metrics(nullptr)
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
	delete metrics;
	delete templates;
	delete overlays;
	delete smudges;
	delete technos;
	delete buildings;
}

void Map::AddBibRA(Gdiplus::Point location, Structure* s)
{
	if (s->type->Bib)
	{
		SmudgeType* smudgeType = from(SmudgeType::SmudgeMapRA).where([](std::pair<std::string, SmudgeType*> t) {return t.second->Flag == SMUDGETYPE_BIB1; }).firstOrDefault().second;
		SmudgeType* smudgeType2 = from(SmudgeType::SmudgeMapRA).where([](std::pair<std::string, SmudgeType*> t) {return t.second->Flag == SMUDGETYPE_BIB2; }).firstOrDefault().second;
		SmudgeType* smudgeType3 = from(SmudgeType::SmudgeMapRA).where([](std::pair<std::string, SmudgeType*> t) {return t.second->Flag == SMUDGETYPE_BIB3; }).firstOrDefault().second;
		SmudgeType* smudgeType4 = nullptr;
		switch (s->type->Size.Width)
		{
		case 2:
			smudgeType4 = smudgeType3;
			break;
		case 3:
			smudgeType4 = smudgeType2;
			break;
		case 4:
			smudgeType4 = smudgeType;
			break;
		}
		if (smudgeType4 == nullptr)
		{
			return;
		}
		int num = 0;
		for (int i = 0; i < smudgeType4->Size.Height; i++)
		{
			int num2 = 0;
			while (num2 < smudgeType4->Size.Width)
			{
				int cell;
				if (metrics->GetCell(Gdiplus::Point(location.X + num2, location.Y + s->type->Size.Height + i - 1), cell))
				{
					Smudge sm;
					sm.type = smudgeType4;
					sm.data = 0;
					sm.icon = num;
					smudges->Set(cell, sm);
					s->BibCells.push_back(cell);
				}
				num2++;
				num++;
			}
		}
	}
}

void Map::AddBibTD(Gdiplus::Point location, Structure* s)
{
	if (s->type->Bib)
	{
		SmudgeType* smudgeType = from(SmudgeType::SmudgeMapTD).where([](std::pair<std::string, SmudgeType*> t) {return t.second->Flag == SMUDGETYPE_BIB1; }).firstOrDefault().second;
		SmudgeType* smudgeType2 = from(SmudgeType::SmudgeMapTD).where([](std::pair<std::string, SmudgeType*> t) {return t.second->Flag == SMUDGETYPE_BIB2; }).firstOrDefault().second;
		SmudgeType* smudgeType3 = from(SmudgeType::SmudgeMapTD).where([](std::pair<std::string, SmudgeType*> t) {return t.second->Flag == SMUDGETYPE_BIB3; }).firstOrDefault().second;
		SmudgeType* smudgeType4 = nullptr;
		switch (s->type->Size.Width)
		{
		case 2:
			smudgeType4 = smudgeType3;
			break;
		case 3:
			smudgeType4 = smudgeType2;
			break;
		case 4:
			smudgeType4 = smudgeType;
			break;
		}
		if (smudgeType4 == nullptr)
		{
			return;
		}
		int num = 0;
		for (int i = 0; i < smudgeType4->Size.Height; i++)
		{
			int num2 = 0;
			while (num2 < smudgeType4->Size.Width)
			{
				int cell;
				if (metrics->GetCell(Gdiplus::Point(location.X + num2, location.Y + s->type->Size.Height + i - 1), cell))
				{
					Smudge sm;
					sm.type = smudgeType4;
					sm.data = 0;
					sm.icon = num;
					smudges->Set(cell, sm);
					s->BibCells.push_back(cell);
				}
				num2++;
				num++;
			}
		}
	}
}

void Map::BuildingsOccupierAddedRA(OccupierEventArgs<Occupier*> args)
{
	if (typeid(*args.Occupier) == typeid(Structure))
	{
		Structure* s = dynamic_cast<Structure*>(args.Occupier);
		AddBibRA(args.Location, s);
	}
}

void Map::BuildingsOccupierAddedTD(OccupierEventArgs<Occupier*> args)
{
	if (typeid(*args.Occupier) == typeid(Structure))
	{
		Structure* s = dynamic_cast<Structure*>(args.Occupier);
		AddBibTD(args.Location, s);
	}
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
		metrics = new CellMetrics(RA_MAP_CELL_W, RA_MAP_CELL_H);
		char th[9];
		ini.Get_String("Map", "Theater", "", th, 9);
		_strlwr(th);
		theater = th;
		theaterid = RATheaterIDs[theater];
		TheTilesetManager->SetSets(RATheaters[theater]);
		char pl[20];
		ini.Get_String("Basic", "Player", "", pl, 20);
		_strlwr(pl);
		player = HouseType::HouseMapRA[pl];
		if (!_stricmp(pl, "USSR") || !_stricmp(pl, "Ukrane") || !_stricmp(pl, "BadGuy"))
		{
			baseplayer = HouseType::HouseMapRA["GoodGuy"];
		}
		else
		{
			baseplayer = HouseType::HouseMapRA["BadGuy"];
		}
		width = RA_MAP_CELL_W;
		height = RA_MAP_CELL_H;
		size = RA_MAP_CELL_TOTAL;
		if (templates)
		{
			delete templates;
		}
		templates = new CellGrid<Template>(metrics);
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
					Template tx;
					tx.type = t;
					templates->Set(i,tx);
				}
			}
		}
		for (int i = 0; i < RA_MAP_CELL_TOTAL; i++)
		{
			unsigned char b;
			cdecomp.Get(&b, sizeof(b));
			if (templates->Get(i).type)
			{
				if (templates->Get(i).type->ID != TEMPLATERA_CLEAR1 && b >= templates->Get(i).type->IconWidth * templates->Get(i).type->IconHeight)
				{
					Template tx;
					templates->Set(i,tx);
				}
				else
				{
					Template tx = templates->Get(i);
					tx.icon = b;
					templates->Set(i,tx);
				}
			}
		}
		if (overlays)
		{
			delete[] overlays;
		}
		overlays = new CellGrid<Overlay>(metrics);
		int olen = ini.Get_UUBlock("OverlayPack", _staging_buffer, sizeof(_staging_buffer));
		BufferStraw obstraw(_staging_buffer, olen);
		LCWStraw odecomp(LCWStraw::DECOMPRESS);
		odecomp.Get_From(&obstraw);
		for (int i = 0; i < RA_MAP_CELL_TOTAL; i++)
		{
			unsigned char c;
			odecomp.Get(&c, sizeof(c));
			const OverlayType* o = nullptr;
			if (c < OVERLAYRA_COUNT)
			{
				o = OverlayType::PointersRA[c];
			}
			if (o != nullptr)
			{
				if (o->Theater & theaterid)
				{
					Overlay ox;
					ox.type = o;
					ox.OccupyMask = o->OccupyMask;
					ox.Width = o->Width;
					ox.Height = o->Height;
					overlays->Set(i,ox);
				}
			}
		}
		if (smudges)
		{
			delete[] smudges;
		}
		smudges = new CellGrid<Smudge>(metrics);
		for (int i = 0; i < ini.Entry_Count("Smudge"); i++)
		{
			const char* cell = ini.Get_Entry("Smudge", i);
			char smudge[100];
			ini.Get_String("Smudge", cell, nullptr, smudge, 100);
			char* name = strtok(smudge, ",");
			int cellid = atoi(strtok(nullptr, ","));
			int data = atoi(strtok(nullptr, ","));
			if (SmudgeType::SmudgeMapRA.count(name))
			{
				Smudge s;
				s.type = SmudgeType::SmudgeMapRA[name];
				s.data = data;
				smudges->Set(cellid,s);
			}
		}
		if (technos)
		{
			delete[] technos;
		}
		technos = new OccupierSet<Occupier *>(metrics);
		for (int i = 0; i < ini.Entry_Count("TERRAIN"); i++)
		{
			const char* cell = ini.Get_Entry("TERRAIN", i);
			int cellid = atoi(cell);
			char terrain[100];
			ini.Get_String("TERRAIN", cell, nullptr, terrain, 100);
			_strlwr(terrain);
			if (TerrainType::TerrainMapRA.count(terrain))
			{
				Terrain *t = new Terrain;
				t->type = TerrainType::TerrainMapRA[terrain];
				t->OccupyMask = t->type->OccupyMask;
				t->Width = t->type->Width;
				t->Height = t->type->Height;
				t->trigger = _strdup("");
				technos->Add(cellid, t);
			}
		}
		if (buildings)
		{
			delete[] buildings;
		}
		buildings = new OccupierSet<Occupier*>(metrics);
		buildings->map = this;
		buildings->OnOccupierAdded = &Map::BuildingsOccupierAddedRA;
		for (int i = 0; i < ini.Entry_Count("STRUCTURES"); i++)
		{
			const char* cell = ini.Get_Entry("STRUCTURES", i);
			char structure[200];
			ini.Get_String("STRUCTURES", cell, nullptr, structure, 200);
			char* house = strtok(structure, ",");
			char* name = strtok(nullptr, ",");
			int strength = atoi(strtok(nullptr, ","));
			int cellid = atoi(strtok(nullptr, ","));
			unsigned char direction = (unsigned char)((atoi(strtok(nullptr, ",")) + 8) & -16);
			char* trigger = strtok(nullptr, ",");
			bool sellable = atoi(strtok(nullptr, ","));
			bool rebuild = atoi(strtok(nullptr, ","));
			_strlwr(name);
			if (StructType::StructMapRA.count(name))
			{
				Structure *s = new Structure;
				s->type = StructType::StructMapRA[name];
				s->OccupyMask = s->type->OccupyMask;
				s->Width = s->type->Width;
				s->Height = s->type->Height;
				s->house = HouseType::HouseMapRA[house];
				s->strength = strength;
				s->direction = from(DirectionType::Types).where([direction](DirectionType d) {return d.ID == direction; }).firstOrDefault();
				s->trigger = trigger;
				s->sellable = sellable;
				s->rebuild = rebuild;
				buildings->Add(cellid, s);
			}
		}
		for (int i = 0; i < ini.Entry_Count("Base"); i++)
		{
			const char* p = ini.Get_Entry("Base", i);
			char base[200];
			ini.Get_String("Base", p, nullptr, base, 200);
			if (_stricmp(p, "Count") && strlen(p) == 3)
			{
				int priority = atoi(p);
				char* name = strtok(base, ",");
				int cellid = atoi(strtok(nullptr, ","));
				_strlwr(name);
				StructType* type = StructType::StructMapRA[name];
				if (type)
				{
					Gdiplus::Point location;
					metrics->GetLocation(cellid, location);
					Structure* s = (Structure*)from(buildings->GetOccupiers(typeid(Structure))).where([location](std::pair<Occupier *, Gdiplus::Point> x) {return x.second.Equals(location); }).firstOrDefault().first;
					if (s)
					{
						s->basePriority = priority;
					}
					else
					{
						Structure* s2 = new Structure;
						s2->type = type;
						s2->strength = 256;
						s2->direction = DirectionType::Types[0];
						s2->basePriority = priority;
						s2->OccupyMask = s2->type->OccupyMask;
						s2->Width = s2->type->Width;
						s2->Height = s2->type->Height;
						s2->isPrebuilt = false;
						s2->house = baseplayer;
						buildings->Add(cellid, s2);
					}
				}
			}
			else if (!_stricmp(p, "Player"))
			{
				baseplayer = HouseType::HouseMapRA[base];
			}
		}
	}
	else
	{
		metrics = new CellMetrics(TD_MAP_CELL_W, TD_MAP_CELL_H);
		char th[9];
		ini.Get_String("Map", "Theater", "", th, 9);
		theater = th;
		theaterid = TDTheaterIDs[theater];
		TheTilesetManager->SetSets(TDTheaters[theater]);
		bin.Open();
		char pl[20];
		ini.Get_String("Basic", "Player", "", pl, 20);
		_strlwr(pl);
		player = HouseType::HouseMapTD[pl];
		if (!_stricmp(pl, "BadGuy"))
		{
			baseplayer = HouseType::HouseMapTD["GoodGuy"];
		}
		else
		{
			baseplayer = HouseType::HouseMapTD["BadGuy"];
		}
		width = TD_MAP_CELL_W;
		height = TD_MAP_CELL_H;
		size = TD_MAP_CELL_TOTAL;
		if (templates)
		{
			delete templates;
		}
		templates = new CellGrid<Template>(metrics);
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
					Template tx;
					tx.type = t;
					tx.icon = icon;
					templates->Set(i, tx);
				}
			}
		}
		if (overlays)
		{
			delete[] overlays;
		}
		overlays = new CellGrid<Overlay>(metrics);
		for (int i = 0; i < ini.Entry_Count("Overlay"); i++)
		{
			const char* cell = ini.Get_Entry("Overlay", i);
			int cellid = atoi(cell);
			char overlay[9];
			ini.Get_String("Overlay", cell, nullptr, overlay, 9);
			if (OverlayType::OverlayMapTD.count(overlay))
			{
				Overlay ox;
				ox.type = OverlayType::OverlayMapTD[overlay];
				ox.OccupyMask = ox.type->OccupyMask;
				ox.Width = ox.type->Width;
				ox.Height = ox.type->Height;
				overlays->Set(cellid, ox);
			}
		}
		if (smudges)
		{
			delete[] smudges;
		}
		smudges = new CellGrid<Smudge>(metrics);
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
				Smudge s;
				s.type = SmudgeType::SmudgeMapRA[name];
				s.data = data;
				smudges->Set(cellid, s);
			}
		}
		if (technos)
		{
			delete[] technos;
		}
		technos = new OccupierSet<Occupier *>(metrics);
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
				Terrain *t = new Terrain;
				t->type = TerrainType::TerrainMapTD[name];
				t->trigger = _strdup(trigger);
				t->OccupyMask = t->type->OccupyMask;
				t->Width = t->type->Width;
				t->Height = t->type->Height;
				if (t->type->IsTransformable)
				{
					t->icon = 22;
				}
				technos->Add(cellid, t);
			}
		}
		if (buildings)
		{
			delete[] buildings;
		}
		buildings = new OccupierSet<Occupier*>(metrics);
		buildings->map = this;
		buildings->OnOccupierAdded = &Map::BuildingsOccupierAddedTD;
		for (int i = 0; i < ini.Entry_Count("Structures"); i++)
		{
			const char* cell = ini.Get_Entry("Structures", i);
			char structure[200];
			ini.Get_String("Structures", cell, nullptr, structure, 200);
			char* house = strtok(structure, ",");
			char* name = strtok(nullptr, ",");
			int strength = atoi(strtok(nullptr, ","));
			int cellid = atoi(strtok(nullptr, ","));
			unsigned char direction = (unsigned char)((atoi(strtok(nullptr, ",")) + 8) & -16);
			char* trigger = strtok(nullptr, ",");
			_strlwr(name);
			if (StructType::StructMapTD.count(name))
			{
				Structure* s = new Structure;
				s->type = StructType::StructMapTD[name];
				s->OccupyMask = s->type->OccupyMask;
				s->Width = s->type->Width;
				s->Height = s->type->Height;
				s->house = HouseType::HouseMapTD[house];
				s->strength = strength;
				s->direction = from(DirectionType::Types).where([direction](DirectionType d) {return d.ID == direction; }).firstOrDefault();
				s->trigger = trigger;
				buildings->Add(cellid, s);
			}
		}
		for (int i = 0; i < ini.Entry_Count("Base"); i++)
		{
			const char* p = ini.Get_Entry("Base", i);
			char base[200];
			ini.Get_String("Base", p, nullptr, base, 200);
			if (_stricmp(p, "Player") && _stricmp(p, "Count") && strlen(p) == 3)
			{
				int priority = atoi(p);
				char* name = strtok(base, ",");
				int cellid = atoi(strtok(nullptr, ","));
				_strlwr(name);
				StructType* type = StructType::StructMapTD[name];
				if (type)
				{
					Gdiplus::Point location = Gdiplus::Point((cellid >> 8) & 0x3F, (cellid >> 24) & 0x3F);
					Structure* s = (Structure*)from(buildings->GetOccupiers(typeid(Structure))).where([location](std::pair<Occupier*, Gdiplus::Point> x) {return x.second.Equals(location); }).firstOrDefault().first;
					if (s)
					{
						s->basePriority = priority;
					}
					else
					{
						Structure* s2 = new Structure;
						s2->type = type;
						s2->strength = 256;
						s2->direction = DirectionType::Types[0];
						s2->basePriority = priority;
						s2->OccupyMask = s2->type->OccupyMask;
						s2->Width = s2->type->Width;
						s2->Height = s2->type->Height;
						s2->isPrebuilt = false;
						s2->house = baseplayer;
						buildings->Add(cellid, s2);
					}
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
			Overlay o = overlays->Get(cell);
			if (o.type)
			{
				if (o.type->Flag == OVERLAYTYPE_TIBERIUMORGOLD || o.type->Flag == OVERLAYTYPE_GEMS)
				{
					int count = 0;
					for (auto i : metrics->AdjacentFacings)
					{
						Overlay ao = overlays->Adjacent(cell, i);
						if (ao.type && (ao.type->Flag == OVERLAYTYPE_TIBERIUMORGOLD || ao.type->Flag == OVERLAYTYPE_GEMS))
						{
							count++;
						}
					}
					if (o.type->Flag == OVERLAYTYPE_TIBERIUMORGOLD)
					{
						o.icon = icons1[count];
					}
					else
					{
						o.icon = icons2[count];
					}
					overlays->Set(cell, o);
				}
				if (o.type->Flag == OVERLAYTYPE_WALL)
				{
					Overlay north = overlays->Adjacent(cell, FACING_NORTH);
					Overlay east = overlays->Adjacent(cell, FACING_EAST);
					Overlay south = overlays->Adjacent(cell, FACING_SOUTH);
					Overlay west = overlays->Adjacent(cell, FACING_WEST);
					int count = 0;
					if (north.type == o.type)
					{
						count |= 1;
					}
					if (east.type == o.type)
					{
						count |= 2;
					}
					if (south.type == o.type)
					{
						count |= 4;
					}
					if (west.type == o.type)
					{
						count |= 8;
					}
					o.icon = count;
					overlays->Set(cell, o);
				}
			}
		}
	}
}

void Map::Render(Gdiplus::Graphics *graphics)
{
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
	static Gdiplus::Point TurretAdjust[32]
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
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int cell = (i * width) + j;
			Template t = templates->Get(cell);
			const char* name = "clear1";
			int icon = ((cell & 3) | ((cell >> 4) & 12));
			if (t.type)
			{
				name = t.type->Name.c_str();
				icon = t.icon;
			}
			Tile* tile;
			if (TheTilesetManager->GetTileData(name, icon, tile))
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
			Smudge o = smudges->Get(cell);
			const char* name = nullptr;
			int icon = 0;
			if (o.type)
			{
				name = o.type->Name.c_str();
				icon = o.icon;
			}
			if (name)
			{
				Tile* tile;
				if (TheTilesetManager->GetTileData(name, icon, tile))
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
			Overlay o = overlays->Get(cell);
			const char* name = nullptr;
			int icon = 0;
			if (o.type)
			{
				name = o.type->Name.c_str();
				if (o.type->Flag == OVERLAYTYPE_TIBERIUMORGOLD && !isra)
				{
					name = OverlayType::Tiberium[rand() % OverlayType::Tiberium.size()]->Name.c_str();
				}
				if (o.type->Flag == OVERLAYTYPE_TIBERIUMORGOLD && isra)
				{
					name = OverlayType::Gold[rand() % OverlayType::Gold.size()]->Name.c_str();
				}
				if (o.type->Flag == OVERLAYTYPE_GEMS)
				{
					name = OverlayType::Gems[rand() % OverlayType::Gems.size()]->Name.c_str();
				}
				icon = o.icon;
			}
			if (name)
			{
				Tile* tile;
				if (TheTilesetManager->GetTileData(name, icon, tile))
				{
					int twidth = tilesize;
					int theight = tilesize;
					if (o.type->Flag == OVERLAYTYPE_CRATE || o.type->Flag == OVERLAYTYPE_FLAG)
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
	std::vector<std::pair<Occupier *, Gdiplus::Point>> occupiers = technos->GetOccupiers(typeid(Terrain));
	for (auto i : occupiers)
	{
		Terrain *o = (Terrain *)i.first;
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
				int twidth = tile->Image->GetWidth() / tilescale;
				int theight = tile->Image->GetHeight() / tilescale;
				int xpos = p.X * tilesize;
				int ypos = p.Y * tilesize;
				Gdiplus::Rect r;
				r.X = xpos;
				r.Y = ypos;
				r.Width = twidth;
				r.Height = theight;
				graphics->DrawImage(tile->Image, r);
			}
		}
	}
	std::vector<std::pair<Occupier*, Gdiplus::Point>> boccupiers = buildings->GetOccupiers(typeid(Structure));
	for (auto i : boccupiers)
	{
		Structure* building = (Structure *)i.first;
		Gdiplus::Point topLeft = i.second;
		Gdiplus::StringFormat stringFormat;
		stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		Gdiplus::SolidBrush fakeBackgroundBrush = Gdiplus::SolidBrush(Gdiplus::Color(64, 0, 0, 0));
		Gdiplus::SolidBrush fakeTextBrush = Gdiplus::SolidBrush(Gdiplus::Color(128, 255, 255, 255));
		Gdiplus::SolidBrush baseBackgroundBrush = Gdiplus::SolidBrush(Gdiplus::Color(64, 0, 0, 0));
		Gdiplus::SolidBrush baseTextBrush = Gdiplus::SolidBrush(Gdiplus::Color(128, 255, 0, 0));
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
			Gdiplus::Point location(topLeft.X * tilesize, topLeft.Y * tilesize);
			Gdiplus::Size size1(tile->Image->GetWidth() / tilescale, tile->Image->GetHeight() / tilescale);
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
				Gdiplus::RectF rectangleF(Gdiplus::PointF((float)buildingBounds.X, (float)buildingBounds.Y), size2);
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
				p = p + Gdiplus::Size((int)(((float)buildingBounds.Width - size3.Width) / 2), (int)((float)buildingBounds.Height - size3.Height));
				Gdiplus::RectF rectangleF2(Gdiplus::PointF((float)p.X, (float)p.Y), size3);
				graphics->FillRectangle(&baseBackgroundBrush, rectangleF2);
				graphics->DrawString(text2, wcslen(text2), &font, rectangleF2, &stringFormat, &baseTextBrush);
				ReleaseDC(nullptr, dc);
			}
		}
	}
}
