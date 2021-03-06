#include "global.h"
#include "global.h"
#include "cellmetrics.h"
#include "map.h"
#include "tileset.h"
#include "TemplateType.h"
#include "overlaytype.h"
#include "smudgetype.h"
#include "TerrainType.h"
#include "structtype.h"
#include "AircraftType.h"
#include "vesseltype.h"
#include "unittype.h"
#include "infantrytype.h"
#include "INI.H"
#include "RAWFILE.H"
#include "XSTRAW.H"
#include "LCWSTRAW.H"
#include <time.h>
#include "boolinq.h"
#include "housetype.h"
#include "teamcolor.h"
#include "maprender.h"
#include "mappanel.h"
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
const FacingType CellMetrics::AdjacentFacings[8] = { FACING_N, FACING_NE, FACING_E, FACING_SE, FACING_S, FACING_SW, FACING_W, FACING_NW };
DirectionType DirectionType::Types[16] = {
	{0, "North", FACING_N},
	{16, "North-NorthEast", FACING_NONE},
	{32, "NorthEast", FACING_NE},
	{48, "East-NorthEast", FACING_NONE},
	{64, "East", FACING_E},
	{80, "East-SouthEast", FACING_NONE},
	{96, "SouthEast", FACING_SE},
	{112, "South-SouthEast", FACING_NONE},
	{128, "South", FACING_S},
	{144, "South-SouthWest", FACING_NONE},
	{160, "SouthWest", FACING_SW},
	{176, "West-SouthWest", FACING_NONE},
	{192, "West", FACING_W},
	{208, "West-NorthWest", FACING_NONE},
	{224, "NorthWest", FACING_NW},
	{240, "North-NorthWest", FACING_NONE}
};
Map::Map() : templates(nullptr), overlays(nullptr), smudges(nullptr), technos(nullptr), buildings(nullptr), isra(false), theaterid(0), metrics(nullptr), updatecount(0), updating(false), invalidateoverlappers(false), baseplayer(nullptr), player(nullptr), overlappers(nullptr)
{
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
	buildings->ClearOccupiers();
	delete buildings;
	delete overlappers;
}

void Map::AddBib(Gdiplus::Point location, Structure* s)
{
	auto smudgemap = SmudgeType::SmudgeMapTD;
	if (isra)
	{
		smudgemap = SmudgeType::SmudgeMapRA;
	}
	if (s->type->HasBib)
	{
		SmudgeType* smudgeType = from(smudgemap).where([](std::pair<std::string, SmudgeType*> t) {return t.second->Flag == SMUDGETYPE_BIB1; }).firstOrDefault().second;
		SmudgeType* smudgeType2 = from(smudgemap).where([](std::pair<std::string, SmudgeType*> t) {return t.second->Flag == SMUDGETYPE_BIB2; }).firstOrDefault().second;
		SmudgeType* smudgeType3 = from(smudgemap).where([](std::pair<std::string, SmudgeType*> t) {return t.second->Flag == SMUDGETYPE_BIB3; }).firstOrDefault().second;
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
					Smudge* sm = new Smudge;
					sm->type = smudgeType4;
					sm->data = 0;
					sm->icon = num;
					sm->OverlapBounds = sm->type->OverlapBounds;
					smudges->Set(cell, sm);
					s->BibCells.insert(cell);
				}
				num2++;
				num++;
			}
		}
	}
}

void Map::BuildingsOccupierAdded(OccupierEventArgs<Occupier*> args)
{
	Structure* s = dynamic_cast<Structure*>(args.Occupier);
	if (s)
	{
		technos->Add(args.Location, args.Occupier, s->type->BaseOccupyMask, s->type->Width, s->type->Height);
		AddBib(args.Location, s);
	}
	else
	{
		technos->Add(args.Location, args.Occupier);
	}
}

void Map::Load(const char* path)
{
	BeginUpdate();
	srand((unsigned int)time(nullptr) + GetCurrentProcessId());
	RawFileClass f(path);
	f.Open();
	INIClass ini;
	ini.Load(f);
	char bpath[MAX_PATH];
	strcpy(bpath, path);
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
		if (overlappers)
		{
			delete overlappers;
		}
		overlappers = new OverlapperSet<Overlapper*>(metrics);
		if (templates)
		{
			delete templates;
		}
		templates = new CellGrid<Template*>(metrics, this);
		if (overlays)
		{
			delete overlays;
		}
		overlays = new CellGrid<Overlay*>(metrics, this);
		overlays->OnCellChanged = &Map::OverlayCellChanged;
		if (smudges)
		{
			delete smudges;
		}
		smudges = new CellGrid<Smudge*>(metrics, this);
		smudges->OnCellChanged = &Map::SmudgeCellChanged;
		if (technos)
		{
			delete technos;
		}
		technos = new OccupierSet<Occupier*>(metrics);
		technos->map = this;
		technos->OnOccupierAdded = &Map::TechnosOccupierAdded;
		technos->OnOccupierRemoved = &Map::TechnosOccupierRemoved;
		if (buildings)
		{
			buildings->ClearOccupiers();
			delete buildings;
		}
		buildings = new OccupierSet<Occupier*>(metrics);
		buildings->map = this;
		buildings->OnOccupierAdded = &Map::BuildingsOccupierAdded;
		buildings->OnOccupierRemoved = &Map::BuildingsOccupierRemoved;
		char th[12];
		ini.Get_String("Map", "Theater", "", th, 12);
		x = ini.Get_Int("Map", "X", 0);
		y = ini.Get_Int("Map", "Y", 0);
		width = ini.Get_Int("Map", "Width", 0);
		height = ini.Get_Int("Map", "Height", 0);
		_strlwr(th);
		theater = th;
		theaterid = RATheaterIDs[theater];
		TheTilesetManager->Reset();
		TheTilesetManager->SetSets(RATheaters[theater]);
		char pl[20];
		ini.Get_String("Basic", "Player", "", pl, 20);
		_strlwr(pl);
		player = HouseType::HouseMapRA[pl];
		if (_stricmp(pl, "USSR") && _stricmp(pl, "Ukrane") && _stricmp(pl, "BadGuy"))
		{
			baseplayer = HouseType::HouseMapRA["BadGuy"];
		}
		else
		{
			baseplayer = HouseType::HouseMapRA["GoodGuy"];
		}
		MainPanel->data = new BYTE[TileWidth * metrics->Width * TileHeight * metrics->Height * 4];
		MainPanel->mapImage = new Gdiplus::Bitmap(TileWidth * metrics->Width, TileHeight * metrics->Height, TileWidth * metrics->Width * 4, PixelFormat32bppARGB, MainPanel->data);
		Free();
		Init();
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
					Template* tx = new Template;
					tx->type = t;
					templates->Set(i,tx);
				}
			}
		}
		for (int i = 0; i < RA_MAP_CELL_TOTAL; i++)
		{
			unsigned char b;
			cdecomp.Get(&b, sizeof(b));
			if (templates->Get(i) && templates->Get(i)->type)
			{
				if (templates->Get(i)->type->ID != TEMPLATERA_CLEAR1 && b >= templates->Get(i)->type->IconWidth * templates->Get(i)->type->IconHeight)
				{
					templates->Set(i,nullptr);
				}
				else
				{
					Template* tx = templates->Get(i);
					tx->icon = b;
				}
			}
		}
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
					Overlay* ox = new Overlay;
					ox->type = o;
					ox->OccupyMask = o->OccupyMask;
					ox->Width = o->Width;
					ox->Height = o->Height;
					overlays->Set(i,ox);
				}
			}
		}
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
				Smudge* s = new Smudge;
				s->type = SmudgeType::SmudgeMapRA[name];
				s->data = data;
				s->OverlapBounds = s->type->OverlapBounds;
				smudges->Set(cellid, s);
			}
		}
		for (int i = 0; i < ini.Entry_Count("TERRAIN"); i++)
		{
			const char* cell = ini.Get_Entry("TERRAIN", i);
			int cellid = atoi(cell);
			char terrain[100];
			ini.Get_String("TERRAIN", cell, nullptr, terrain, 100);
			_strlwr(terrain);
			if (TerrainType::TerrainMapRA.count(terrain))
			{
				Terrain* t = new Terrain;
				t->type = TerrainType::TerrainMapRA[terrain];
				t->OccupyMask = t->type->OccupyMask;
				t->Width = t->type->Size.Width;
				t->Height = t->type->Size.Height;
				t->trigger = _strdup("");
				t->OverlapBounds = t->type->OverlapBounds;
				technos->Add(cellid, t);
			}
		}
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
				Structure* s = new Structure;
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
				s->OverlapBounds = s->type->OverlapBounds;
				buildings->Add(cellid, s);
			}
		}
		for (int i = 0; i < ini.Entry_Count("AIRCRAFT"); i++)
		{
			const char* cell = ini.Get_Entry("AIRCRAFT", i);
			char aircraft[200];
			ini.Get_String("AIRCRAFT", cell, nullptr, aircraft, 200);
			char* house = strtok(aircraft, ",");
			char* name = strtok(nullptr, ",");
			int strength = atoi(strtok(nullptr, ","));
			int cellid = atoi(strtok(nullptr, ","));
			unsigned char direction = (unsigned char)((atoi(strtok(nullptr, ",")) + 8) & -16);
			char* mission = strtok(nullptr, ",");
			_strlwr(name);
			if (AircraftType::AircraftMapRA.count(name))
			{
				Aircraft* a = new Aircraft;
				a->type = AircraftType::AircraftMapRA[name];
				a->OccupyMask = a->type->OccupyMask;
				a->Width = a->type->Width;
				a->Height = a->type->Height;
				a->house = HouseType::HouseMapRA[house];
				a->strength = strength;
				a->direction = from(DirectionType::Types).where([direction](DirectionType d) {return d.ID == direction; }).firstOrDefault();
				a->mission = mission;
				a->OverlapBounds = a->type->OverlapBounds;
				technos->Add(cellid, a);
			}
		}
		for (int i = 0; i < ini.Entry_Count("INFANTRY"); i++)
		{
			const char* cell = ini.Get_Entry("INFANTRY", i);
			char aircraft[200];
			ini.Get_String("INFANTRY", cell, nullptr, aircraft, 200);
			char* house = strtok(aircraft, ",");
			char* name = strtok(nullptr, ",");
			int strength = atoi(strtok(nullptr, ","));
			int cellid = atoi(strtok(nullptr, ","));
			int stopping = atoi(strtok(nullptr, ","));
			char* mission = strtok(nullptr, ",");
			unsigned char direction = (unsigned char)((atoi(strtok(nullptr, ",")) + 8) & -16);
			char* trigger = strtok(nullptr, ",");
			_strlwr(name);
			if (InfantryType::InfantryMapRA.count(name))
			{
				Occupier* o = technos->Get(cellid);
				InfantryGroup* group = dynamic_cast<InfantryGroup *>(o);
				if (!group && !o)
				{
					group = new InfantryGroup();
					technos->Add(cellid, group);
				}
				if (group)
				{
					group->infantry[stopping] = new Infantry;
					group->infantry[stopping]->group = group;
					group->infantry[stopping]->type = InfantryType::InfantryMapRA[name];
					group->infantry[stopping]->house = HouseType::HouseMapRA[house];
					group->infantry[stopping]->strength = strength;
					group->infantry[stopping]->direction = from(DirectionType::Types).where([direction](DirectionType d) {return d.ID == direction; }).firstOrDefault();
					group->infantry[stopping]->mission = mission;
					group->infantry[stopping]->trigger = trigger;
				}
			}
		}
		for (int i = 0; i < ini.Entry_Count("SHIPS"); i++)
		{
			const char* cell = ini.Get_Entry("SHIPS", i);
			char vessel[200];
			ini.Get_String("SHIPS", cell, nullptr, vessel, 200);
			char* house = strtok(vessel, ",");
			char* name = strtok(nullptr, ",");
			int strength = atoi(strtok(nullptr, ","));
			int cellid = atoi(strtok(nullptr, ","));
			unsigned char direction = (unsigned char)((atoi(strtok(nullptr, ",")) + 8) & -16);
			char* mission = strtok(nullptr, ",");
			char* trigger = strtok(nullptr, ",");
			_strlwr(name);
			if (VesselType::VesselMapRA.count(name))
			{
				Vessel* a = new Vessel;
				a->type = VesselType::VesselMapRA[name];
				a->OccupyMask = a->type->OccupyMask;
				a->Width = a->type->Width;
				a->Height = a->type->Height;
				a->house = HouseType::HouseMapRA[house];
				a->strength = strength;
				a->direction = from(DirectionType::Types).where([direction](DirectionType d) {return d.ID == direction; }).firstOrDefault();
				a->mission = mission;
				a->trigger = trigger;
				a->OverlapBounds = a->type->OverlapBounds;
				technos->Add(cellid, a);
			}
		}
		for (int i = 0; i < ini.Entry_Count("UNITS"); i++)
		{
			const char* cell = ini.Get_Entry("UNITS", i);
			char unit[200];
			ini.Get_String("UNITS", cell, nullptr, unit, 200);
			char* house = strtok(unit, ",");
			char* name = strtok(nullptr, ",");
			int strength = atoi(strtok(nullptr, ","));
			int cellid = atoi(strtok(nullptr, ","));
			unsigned char direction = (unsigned char)((atoi(strtok(nullptr, ",")) + 8) & -16);
			char* mission = strtok(nullptr, ",");
			char* trigger = strtok(nullptr, ",");
			_strlwr(name);
			if (UnitType::UnitMapRA.count(name))
			{
				Unit* a = new Unit;
				a->type = UnitType::UnitMapRA[name];
				a->OccupyMask = a->type->OccupyMask;
				a->Width = a->type->Width;
				a->Height = a->type->Height;
				a->house = HouseType::HouseMapRA[house];
				a->strength = strength;
				a->direction = from(DirectionType::Types).where([direction](DirectionType d) {return d.ID == direction; }).firstOrDefault();
				a->mission = mission;
				a->trigger = trigger;
				a->OverlapBounds = a->type->OverlapBounds;
				technos->Add(cellid, a);
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
					Structure* s = from(buildings->GetOccupiers<Structure>()).where([location](std::pair<Structure*, Gdiplus::Point> x) {return x.second.Equals(location); }).firstOrDefault().first;
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
						s2->OverlapBounds = s2->type->OverlapBounds;
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
		if (overlappers)
		{
			delete overlappers;
		}
		overlappers = new OverlapperSet<Overlapper*>(metrics);
		if (templates)
		{
			delete templates;
		}
		templates = new CellGrid<Template*>(metrics, this);
		if (overlays)
		{
			delete overlays;
		}
		overlays = new CellGrid<Overlay*>(metrics, this);
		overlays->OnCellChanged = &Map::OverlayCellChanged;
		if (smudges)
		{
			delete smudges;
		}
		smudges = new CellGrid<Smudge*>(metrics, this);
		smudges->OnCellChanged = &Map::SmudgeCellChanged;
		if (technos)
		{
			delete technos;
		}
		technos = new OccupierSet<Occupier*>(metrics);
		technos->map = this;
		technos->OnOccupierAdded = &Map::TechnosOccupierAdded;
		technos->OnOccupierRemoved = &Map::TechnosOccupierRemoved;
		if (buildings)
		{
			buildings->ClearOccupiers();
			delete buildings;
		}
		buildings = new OccupierSet<Occupier*>(metrics);
		buildings->map = this;
		buildings->OnOccupierAdded = &Map::BuildingsOccupierAdded;
		buildings->OnOccupierRemoved = &Map::BuildingsOccupierRemoved;
		char th[12];
		ini.Get_String("Map", "Theater", "", th, 12);
		x = ini.Get_Int("Map", "X", 0);
		y = ini.Get_Int("Map", "Y", 0);
		width = ini.Get_Int("Map", "Width", 0);
		height = ini.Get_Int("Map", "Height", 0);
		theater = th;
		theaterid = TDTheaterIDs[theater];
		TheTilesetManager->Reset();
		TheTilesetManager->SetSets(TDTheaters[theater]);
		bin.Open();
		char pl[20];
		ini.Get_String("Basic", "Player", "", pl, 20);
		_strlwr(pl);
		player = HouseType::HouseMapTD[pl];
		if (_stricmp(pl, "BadGuy"))
		{
			baseplayer = HouseType::HouseMapTD["BadGuy"];
		}
		else
		{
			baseplayer = HouseType::HouseMapTD["GoodGuy"];
		}
		MainPanel->data = new BYTE[TileWidth * metrics->Width * TileHeight * metrics->Height * 4];
		MainPanel->mapImage = new Gdiplus::Bitmap(TileWidth * metrics->Width, TileHeight * metrics->Height, TileWidth * metrics->Width * 4, PixelFormat32bppARGB, MainPanel->data);
		Free();
		Init();
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
					Template* tx = new Template;
					tx->type = t;
					tx->icon = icon;
					templates->Set(i, tx);
				}
			}
		}
		for (int i = 0; i < ini.Entry_Count("Overlay"); i++)
		{
			const char* cell = ini.Get_Entry("Overlay", i);
			int cellid = atoi(cell);
			char overlay[9];
			ini.Get_String("Overlay", cell, nullptr, overlay, 9);
			if (OverlayType::OverlayMapTD.count(overlay))
			{
				Overlay* ox = new Overlay;
				ox->type = OverlayType::OverlayMapTD[overlay];
				ox->OccupyMask = ox->type->OccupyMask;
				ox->Width = ox->type->Width;
				ox->Height = ox->type->Height;
				overlays->Set(cellid, ox);
			}
		}
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
				Smudge* s = new Smudge;
				s->type = SmudgeType::SmudgeMapRA[name];
				s->data = data;
				s->OverlapBounds = s->type->OverlapBounds;
				smudges->Set(cellid, s);
			}
		}
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
				Terrain* t = new Terrain;
				t->type = TerrainType::TerrainMapTD[name];
				t->trigger = _strdup(trigger);
				t->OccupyMask = t->type->OccupyMask;
				t->Width = t->type->Size.Width;
				t->Height = t->type->Size.Height;
				if (t->type->IsTransformable)
				{
					t->icon = 22;
				}
				t->OverlapBounds = t->type->OverlapBounds;
				technos->Add(cellid, t);
			}
		}
		for (int i = 0; i < ini.Entry_Count("Aircraft"); i++)
		{
			const char* cell = ini.Get_Entry("Aircraft", i);
			char aircraft[200];
			ini.Get_String("Aircraft", cell, nullptr, aircraft, 200);
			char* house = strtok(aircraft, ",");
			char* name = strtok(nullptr, ",");
			int strength = atoi(strtok(nullptr, ","));
			int cellid = atoi(strtok(nullptr, ","));
			unsigned char direction = (unsigned char)((atoi(strtok(nullptr, ",")) + 8) & -16);
			char* mission = strtok(nullptr, ",");
			_strlwr(name);
			if (AircraftType::AircraftMapTD.count(name))
			{
				Aircraft* a = new Aircraft;
				a->type = AircraftType::AircraftMapTD[name];
				a->OccupyMask = a->type->OccupyMask;
				a->Width = a->type->Width;
				a->Height = a->type->Height;
				a->house = HouseType::HouseMapTD[house];
				a->strength = strength;
				a->direction = from(DirectionType::Types).where([direction](DirectionType d) {return d.ID == direction; }).firstOrDefault();
				a->mission = mission;
				a->OverlapBounds = a->type->OverlapBounds;
				technos->Add(cellid, a);
			}
		}
		for (int i = 0; i < ini.Entry_Count("Infantry"); i++)
		{
			const char* cell = ini.Get_Entry("Infantry", i);
			char aircraft[200];
			ini.Get_String("Infantry", cell, nullptr, aircraft, 200);
			char* house = strtok(aircraft, ",");
			char* name = strtok(nullptr, ",");
			int strength = atoi(strtok(nullptr, ","));
			int cellid = atoi(strtok(nullptr, ","));
			int stopping = atoi(strtok(nullptr, ","));
			char* mission = strtok(nullptr, ",");
			unsigned char direction = (unsigned char)((atoi(strtok(nullptr, ",")) + 8) & -16);
			char* trigger = strtok(nullptr, ",");
			_strlwr(name);
			if (InfantryType::InfantryMapTD.count(name))
			{
				Occupier* o = technos->Get(cellid);
				InfantryGroup* group = dynamic_cast<InfantryGroup*>(o);
				if (!group && !o)
				{
					group = new InfantryGroup();
					technos->Add(cellid, group);
				}
				if (group)
				{
					group->infantry[stopping] = new Infantry;
					group->infantry[stopping]->group = group;
					group->infantry[stopping]->type = InfantryType::InfantryMapTD[name];
					group->infantry[stopping]->house = HouseType::HouseMapTD[house];
					group->infantry[stopping]->strength = strength;
					group->infantry[stopping]->direction = from(DirectionType::Types).where([direction](DirectionType d) {return d.ID == direction; }).firstOrDefault();
					group->infantry[stopping]->mission = mission;
					group->infantry[stopping]->trigger = trigger;
				}
			}
		}
		for (int i = 0; i < ini.Entry_Count("Units"); i++)
		{
			const char* cell = ini.Get_Entry("Units", i);
			char unit[200];
			ini.Get_String("Units", cell, nullptr, unit, 200);
			char* house = strtok(unit, ",");
			char* name = strtok(nullptr, ",");
			int strength = atoi(strtok(nullptr, ","));
			int cellid = atoi(strtok(nullptr, ","));
			unsigned char direction = (unsigned char)((atoi(strtok(nullptr, ",")) + 8) & -16);
			char* mission = strtok(nullptr, ",");
			char* trigger = strtok(nullptr, ",");
			_strlwr(name);
			if (UnitType::UnitMapTD.count(name))
			{
				Unit* a = new Unit;
				a->type = UnitType::UnitMapTD[name];
				a->OccupyMask = a->type->OccupyMask;
				a->Width = a->type->Width;
				a->Height = a->type->Height;
				a->house = HouseType::HouseMapTD[house];
				a->strength = strength;
				a->direction = from(DirectionType::Types).where([direction](DirectionType d) {return d.ID == direction; }).firstOrDefault();
				a->mission = mission;
				a->trigger = trigger;
				a->OverlapBounds = a->type->OverlapBounds;
				technos->Add(cellid, a);
			}
		}
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
				s->OverlapBounds = s->type->OverlapBounds;
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
					Structure* s = from(buildings->GetOccupiers<Structure>()).where([location](std::pair<Structure*, Gdiplus::Point> x) {return x.second.Equals(location); }).firstOrDefault().first;
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
						s2->OverlapBounds = s2->type->OverlapBounds;
						buildings->Add(cellid, s2);
					}
				}
			}
		}
	}
	EndUpdate();
}

void Map::Update()
{
	updating = true;
	std::set<Gdiplus::Point> value;
	if (TryGetValue(invalidatelayers, MAPLAYER_RESOURCES, value))
	{
		UpdateResourceOverlays(value);
	}
	if (TryGetValue(invalidatelayers, MAPLAYER_RESOURCES, value))
	{
		UpdateWallOverlays(value);
	}
	if (invalidateoverlappers)
	{
		overlappers->Clear();
		for (int i = 0; i < metrics->Length; i++)
		{
			Smudge* s = smudges->Get(i);
			if (s)
			{
				overlappers->Add(i, s);
			}
		}
		for (auto techno : *technos)
		{
			Overlapper* overlapper = dynamic_cast<Overlapper*>(techno.first);
			if (overlapper)
			{
				overlappers->Add(techno.second, overlapper);
			}
		}
	}
	invalidatelayers.clear();
	invalidateoverlappers = false;
	updating = false;
}

void Map::UpdateResourceOverlays(std::set<Gdiplus::Point> locations)
{
	static int array[9] = { 0, 1, 3, 4, 6, 7, 8, 10, 11 };
	static int array2[9] = { 0, 0, 0, 1, 1, 1, 2, 2, 2 };
	for (auto item3 : from(overlays->IntersectsWith(locations)).where([](std::pair<int, Overlay*> o) {return o.second->type->IsResource; }).toStdVector())
	{
		int item = item3.first;
		Overlay* item2 = item3.second;
		int num = 0;
		for (auto facing : metrics->AdjacentFacings)
		{
			Overlay* o = overlays->Adjacent(item, facing);
			if (o && o->type->IsResource)
			{
				num++;
			}
		}
		item2->icon = (item2->type->IsGem ? array2[num] : array[num]);
	}
}

void Map::UpdateWallOverlays(std::set<Gdiplus::Point> locations)
{
	for (auto item3 : from(overlays->IntersectsWith(locations)).where([](std::pair<int, Overlay*> o) {return o.second->type->IsWall; }).toStdVector())
	{
		int item = item3.first;
		Overlay* item2 = item3.second;
		Overlay* north = overlays->Adjacent(item, FACING_N);
		Overlay* east = overlays->Adjacent(item, FACING_E);
		Overlay* south = overlays->Adjacent(item, FACING_S);
		Overlay* west = overlays->Adjacent(item, FACING_W);
		int num = 0;
		if (north && north->type == item2->type)
		{
			num |= 1;
		}
		if (east && east->type == item2->type)
		{
			num |= 2;
		}
		if (south && south->type == item2->type)
		{
			num |= 4;
		}
		if (west && west->type == item2->type)
		{
			num |= 8;
		}
		item2->icon = num;
		overlays->Set(item, item2);
	}
}

void Map::RemoveBib(Structure* building)
{
	for (auto b : from(smudges->IntersectsWith(building->BibCells)).where([](std::pair<int, Smudge*> p) {return p.second->type->Flag & SMUDGETYPE_BIB; }).toStdVector())
	{
		delete smudges->Get(b.first);
		smudges->Set(b.first, nullptr);
	}
	building->BibCells.clear();
}

void Map::SmudgeCellChanged(CellChangedEventArgs<Smudge*> e)
{
	if (updatecount == 0)
	{
		if (e.OldValue != nullptr)
		{
			overlappers->Remove(e.OldValue);
		}
		if (e.Value != nullptr)
		{
			overlappers->Add(e.Location, e.Value);
		}
	}
	else
	{
		invalidateoverlappers = true;
	}
}

void Map::OverlayCellChanged(CellChangedEventArgs<Overlay*> e)
{
	if (e.OldValue != nullptr && e.OldValue->type->IsWall)
	{
		buildings->Remove(e.OldValue);
	}
	if (e.Value != nullptr && e.Value->type->IsWall)
	{
		buildings->Add(e.Location, e.Value);
	}
	if (updating)
	{
		return;
	}
	Overlay* o[2] = { e.OldValue,e.Value };
	for (auto overlay : o)
	{
		if (overlay == nullptr)
		{
			continue;
		}
		MapLayerFlag mapLayerFlag = MAPLAYER_NONE;
		if (overlay->type->IsResource)
		{
			mapLayerFlag = MAPLAYER_RESOURCES;
		}
		else
		{
			if (!overlay->type->IsWall)
			{
				continue;
			}
			mapLayerFlag = MAPLAYER_WALLS;
		}
		std::set<Gdiplus::Point> value;
		if (!TryGetValue(invalidatelayers, mapLayerFlag, value))
		{
			invalidatelayers[mapLayerFlag] = value;
		}
		Gdiplus::Rect r(e.Location, Gdiplus::Size(1, 1));
		r.Inflate(1, 1);
		std::set<Gdiplus::Point> p = GetPoints(r);
		UnionWith(value, p);
	}
	if (updatecount == 0)
	{
		Update();
	}
}

void Map::BuildingsOccupierRemoved(OccupierEventArgs<Occupier*> args)
{
	Structure* s = dynamic_cast<Structure*>(args.Occupier);
	if (s)
	{
		RemoveBib(s);
	}
	technos->Remove(args.Occupier);
}

void Map::TechnosOccupierAdded(OccupierEventArgs<Occupier*> args)
{
	Overlapper* overlapper = dynamic_cast<Overlapper*>(args.Occupier);
	if (overlapper)
	{
		if (updatecount == 0)
		{
			overlappers->Add(args.Location, overlapper);
		}
		else
		{
			invalidateoverlappers = true;
		}
	}
}

void Map::TechnosOccupierRemoved(OccupierEventArgs<Occupier*> args)
{
	Overlapper* overlapper = dynamic_cast<Overlapper*>(args.Occupier);
	if (overlapper)
	{
		if (updatecount == 0)
		{
			overlappers->Remove(overlapper);
		}
		else
		{
			invalidateoverlappers = true;
		}
	}
}

void Map::Init()
{
	if (isra)
	{
		for (TemplateType* i : TemplateType::PointersRA)
		{
			if (i)
			{
				i->Init();
			}
		}
		for (std::pair<std::string, SmudgeType*>i : SmudgeType::SmudgeMapRA)
		{
			i.second->Init();
		}
		for (std::pair<std::string, OverlayType*>i : OverlayType::OverlayMapRA)
		{
			i.second->Init();
		}
		for (std::pair<std::string, TerrainType*>i : TerrainType::TerrainMapRA)
		{
			i.second->Init();
		}
		for (std::pair<std::string, StructType*>i : StructType::StructMapRA)
		{
			i.second->Init(isra, from(HouseType::HouseMapRA).where([i](std::pair<std::string, HouseType*> h) {return _stricmp(h.second->Name.c_str(), i.second->OwnerHouse.c_str()) == 0; }).firstOrDefault().second, from(DirectionType::Types).where([](DirectionType d) {return d.Facing == FACING_N; }).first());
		}
		for (std::pair<std::string, AircraftType*>i : AircraftType::AircraftMapRA)
		{
			i.second->Init(isra, from(HouseType::HouseMapRA).where([i](std::pair<std::string, HouseType*> h) {return _stricmp(h.second->Name.c_str(), i.second->OwnerHouse.c_str()) == 0; }).firstOrDefault().second, from(DirectionType::Types).where([](DirectionType d) {return d.Facing == FACING_N; }).first());
		}
		for (std::pair<std::string, VesselType*>i : VesselType::VesselMapRA)
		{
			i.second->Init(isra, from(HouseType::HouseMapRA).where([i](std::pair<std::string, HouseType*> h) {return _stricmp(h.second->Name.c_str(), i.second->OwnerHouse.c_str()) == 0; }).firstOrDefault().second, from(DirectionType::Types).where([](DirectionType d) {return d.Facing == FACING_N; }).first());
		}
		for (std::pair<std::string, UnitType*>i : UnitType::UnitMapRA)
		{
			i.second->Init(isra, from(HouseType::HouseMapRA).where([i](std::pair<std::string, HouseType*> h) {return _stricmp(h.second->Name.c_str(), i.second->OwnerHouse.c_str()) == 0; }).firstOrDefault().second, from(DirectionType::Types).where([](DirectionType d) {return d.Facing == FACING_N; }).first());
		}
		for (std::pair<std::string, InfantryType*>i : InfantryType::InfantryMapRA)
		{
			i.second->Init(isra, from(HouseType::HouseMapRA).where([i](std::pair<std::string, HouseType*> h) {return _stricmp(h.second->Name.c_str(), i.second->OwnerHouse.c_str()) == 0; }).firstOrDefault().second, from(DirectionType::Types).where([](DirectionType d) {return d.Facing == FACING_N; }).first());
		}
	}
	else
	{
		for (TemplateType* i : TemplateType::PointersTD)
		{
			if (i)
			{
				i->Init();
			}
		}
		for (std::pair<std::string, SmudgeType*>i : SmudgeType::SmudgeMapTD)
		{
			i.second->Init();
		}
		for (std::pair<std::string, OverlayType*>i : OverlayType::OverlayMapTD)
		{
			i.second->Init();
		}
		for (std::pair<std::string, TerrainType*>i : TerrainType::TerrainMapTD)
		{
			i.second->Init();
		}
		for (std::pair<std::string, StructType*>i : StructType::StructMapTD)
		{
			i.second->Init(isra, from(HouseType::HouseMapTD).where([i](std::pair<std::string, HouseType*> h) {return _stricmp(h.second->Name.c_str(), i.second->OwnerHouse.c_str()) == 0; }).firstOrDefault().second, from(DirectionType::Types).where([](DirectionType d) {return d.Facing == FACING_N; }).first());
		}
		for (std::pair<std::string, AircraftType*>i : AircraftType::AircraftMapTD)
		{
			i.second->Init(isra, from(HouseType::HouseMapTD).where([i](std::pair<std::string, HouseType*> h) {return _stricmp(h.second->Name.c_str(), i.second->OwnerHouse.c_str()) == 0; }).firstOrDefault().second, from(DirectionType::Types).where([](DirectionType d) {return d.Facing == FACING_N; }).first());
		}
		for (std::pair<std::string, UnitType*>i : UnitType::UnitMapTD)
		{
			i.second->Init(isra, from(HouseType::HouseMapTD).where([i](std::pair<std::string, HouseType*> h) {return _stricmp(h.second->Name.c_str(), i.second->OwnerHouse.c_str()) == 0; }).firstOrDefault().second, from(DirectionType::Types).where([](DirectionType d) {return d.Facing == FACING_N; }).first());
		}
		for (std::pair<std::string, InfantryType*>i : InfantryType::InfantryMapTD)
		{
			i.second->Init(isra, from(HouseType::HouseMapTD).where([i](std::pair<std::string, HouseType*> h) {return _stricmp(h.second->Name.c_str(), i.second->OwnerHouse.c_str()) == 0; }).firstOrDefault().second, from(DirectionType::Types).where([](DirectionType d) {return d.Facing == FACING_N; }).first());
		}
	}
}

void Map::Free()
{
	for (TemplateType* i : TemplateType::PointersRA)
	{
		if (i)
		{
			i->Free();
		}
	}
	for (std::pair<std::string, SmudgeType*>i : SmudgeType::SmudgeMapRA)
	{
		i.second->Free();
	}
	for (std::pair<std::string, OverlayType*>i : OverlayType::OverlayMapRA)
	{
		i.second->Free();
	}
	for (std::pair<std::string, TerrainType*>i : TerrainType::TerrainMapRA)
	{
		i.second->Free();
	}
	for (std::pair<std::string, StructType*>i : StructType::StructMapRA)
	{
		i.second->Free();
	}
	for (std::pair<std::string, AircraftType*>i : AircraftType::AircraftMapRA)
	{
		i.second->Free();
	}
	for (std::pair<std::string, VesselType*>i : VesselType::VesselMapRA)
	{
		i.second->Free();
	}
	for (std::pair<std::string, UnitType*>i : UnitType::UnitMapRA)
	{
		i.second->Free();
	}
	for (std::pair<std::string, InfantryType*>i : InfantryType::InfantryMapRA)
	{
		i.second->Free();
	}
	for (TemplateType* i : TemplateType::PointersTD)
	{
		if (i)
		{
			i->Free();
		}
	}
	for (std::pair<std::string, SmudgeType*>i : SmudgeType::SmudgeMapTD)
	{
		i.second->Free();
	}
	for (std::pair<std::string, OverlayType*>i : OverlayType::OverlayMapTD)
	{
		i.second->Free();
	}
	for (std::pair<std::string, TerrainType*>i : TerrainType::TerrainMapTD)
	{
		i.second->Free();
	}
	for (std::pair<std::string, StructType*>i : StructType::StructMapTD)
	{
		i.second->Free();
	}
	for (std::pair<std::string, AircraftType*>i : AircraftType::AircraftMapTD)
	{
		i.second->Free();
	}
	for (std::pair<std::string, UnitType*>i : UnitType::UnitMapTD)
	{
		i.second->Free();
	}
	for (std::pair<std::string, InfantryType*>i : InfantryType::InfantryMapTD)
	{
		i.second->Free();
	}
}

static Gdiplus::Point stoppingLocations[5]{ {12, 12}, {6, 6}, {18, 6}, {6, 18}, {18, 18} };

std::vector<InfantryStoppingType> InfantryGroup::ClosestStoppingTypes(Gdiplus::Point subPixel)
{
	std::pair<InfantryStoppingType, float> array[5];
	for (int i = 0; i < 5; i++)
	{
		array[i] = std::make_pair((InfantryStoppingType)i, Vector2(static_cast<float>(subPixel.X - stoppingLocations[i].X), static_cast<float>(subPixel.Y - stoppingLocations[i].Y)).LengthSquared());
	}
	return from(array).orderBy([](std::pair<InfantryStoppingType, float> p) {return p.second; }).select([](std::pair<InfantryStoppingType, float> p) {return p.first; }).toStdVector();
}

Map* Map::Clone()
{
	Map* map = new Map();
	map->BeginUpdate();
	map->theater = theater;
	map->x = x;
	map->y = y;
	map->width = width;
	map->height = height;
	map->theaterid = theaterid;
	map->player = player;
	map->baseplayer = baseplayer;
	map->metrics = new CellMetrics(metrics->Width, metrics->Height);
	map->overlappers = new OverlapperSet<Overlapper*>(map->metrics);
	map->templates = new CellGrid<Template*>(map->metrics, map);
	map->overlays = new CellGrid<Overlay*>(map->metrics, map);
	map->overlays->OnCellChanged = &Map::OverlayCellChanged;
	map->smudges = new CellGrid<Smudge*>(map->metrics, map);
	map->smudges->OnCellChanged = &Map::SmudgeCellChanged;
	map->technos = new OccupierSet<Occupier*>(map->metrics);
	map->technos->map = map;
	map->technos->OnOccupierAdded = &Map::TechnosOccupierAdded;
	map->technos->OnOccupierRemoved = &Map::TechnosOccupierRemoved;
	map->buildings = new OccupierSet<Occupier*>(map->metrics);
	map->buildings->map = map;
	map->buildings->OnOccupierAdded = &Map::BuildingsOccupierAdded;
	map->buildings->OnOccupierRemoved = &Map::BuildingsOccupierRemoved;
	map->isra = isra;
	templates->CopyTo(map->templates);
	overlays->CopyTo(map->overlays);
	smudges->CopyTo(map->smudges);
	for (auto techno : *technos)
	{
		Gdiplus::Point item = techno.second;
		Occupier* item2 = techno.first;
		InfantryGroup* infantryGroup = dynamic_cast<InfantryGroup*>(item2);
		if (infantryGroup)
		{
			InfantryGroup* infantryGroup2 = new InfantryGroup();
			infantryGroup2->infantry[0] = infantryGroup->infantry[0];
			infantryGroup2->infantry[1] = infantryGroup->infantry[1];
			infantryGroup2->infantry[2] = infantryGroup->infantry[2];
			infantryGroup2->infantry[3] = infantryGroup->infantry[3];
			infantryGroup2->infantry[4] = infantryGroup->infantry[4];
			map->technos->Add(item, infantryGroup2);
		}
		else if (dynamic_cast<Structure*>(item2) == nullptr)
		{
			map->technos->Add(item, item2);
		}
	}
	for (auto structure : *buildings)
	{
		Gdiplus::Point item3 = structure.second;
		Occupier *item4 = structure.first;
		map->buildings->Add(item3, item4);
	}
	map->EndUpdate();
	return map;
}