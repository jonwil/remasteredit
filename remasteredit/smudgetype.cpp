#include "global.h"
#include "smudgetype.h"
#include "tileset.h"
std::map<std::string, SmudgeType*> SmudgeType::SmudgeMapTD;
std::map<std::string, SmudgeType*> SmudgeType::SmudgeMapRA;
static SmudgeType CRATER1TD(SMUDGETD_CRATER1, "cr1", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, false);
static SmudgeType CRATER2TD(SMUDGETD_CRATER2, "cr2", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, false);
static SmudgeType CRATER3TD(SMUDGETD_CRATER3, "cr3", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, false);
static SmudgeType CRATER4TD(SMUDGETD_CRATER4, "cr4", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, false);
static SmudgeType CRATER5TD(SMUDGETD_CRATER5, "cr5", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, false);
static SmudgeType CRATER6TD(SMUDGETD_CRATER6, "cr6", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, false);
static SmudgeType SCORCH1TD(SMUDGETD_SCORCH1, "sc1", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, false);
static SmudgeType SCORCH2TD(SMUDGETD_SCORCH2, "sc2", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, false);
static SmudgeType SCORCH3TD(SMUDGETD_SCORCH3, "sc3", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, false);
static SmudgeType SCORCH4TD(SMUDGETD_SCORCH4, "sc4", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, false);
static SmudgeType SCORCH5TD(SMUDGETD_SCORCH5, "sc5", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, false);
static SmudgeType SCORCH6TD(SMUDGETD_SCORCH6, "sc6", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, false);
static SmudgeType BIB1TD(SMUDGETD_BIB1, "bib1", Gdiplus::Size(4, 2), SMUDGETYPE_BIB1, false);
static SmudgeType BIB2TD(SMUDGETD_BIB2, "bib2", Gdiplus::Size(3, 2), SMUDGETYPE_BIB2, false);
static SmudgeType BIB3TD(SMUDGETD_BIB3, "bib3", Gdiplus::Size(2, 2), SMUDGETYPE_BIB3, false);
static SmudgeType CRATER1RA(SMUDGERA_CRATER1, "cr1", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, true);
static SmudgeType CRATER2RA(SMUDGERA_CRATER2, "cr2", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, true);
static SmudgeType CRATER3RA(SMUDGERA_CRATER3, "cr3", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, true);
static SmudgeType CRATER4RA(SMUDGERA_CRATER4, "cr4", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, true);
static SmudgeType CRATER5RA(SMUDGERA_CRATER5, "cr5", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, true);
static SmudgeType CRATER6RA(SMUDGERA_CRATER6, "cr6", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, true);
static SmudgeType SCORCH1RA(SMUDGERA_SCORCH1, "sc1", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, true);
static SmudgeType SCORCH2RA(SMUDGERA_SCORCH2, "sc2", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, true);
static SmudgeType SCORCH3RA(SMUDGERA_SCORCH3, "sc3", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, true);
static SmudgeType SCORCH4RA(SMUDGERA_SCORCH4, "sc4", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, true);
static SmudgeType SCORCH5RA(SMUDGERA_SCORCH5, "sc5", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, true);
static SmudgeType SCORCH6RA(SMUDGERA_SCORCH6, "sc6", Gdiplus::Size(1, 1), SMUDGETYPE_NONE, true);
static SmudgeType BIB1RA(SMUDGERA_BIB1, "bib1", Gdiplus::Size(4, 2), SMUDGETYPE_BIB1, true);
static SmudgeType BIB2RA(SMUDGERA_BIB2, "bib2", Gdiplus::Size(3, 2), SMUDGETYPE_BIB2, true);
static SmudgeType BIB3RA(SMUDGERA_BIB3, "bib3", Gdiplus::Size(2, 2), SMUDGETYPE_BIB3, true);
void SmudgeType::Init()
{
	Tile *tile;
	if (TheTilesetManager->GetTileData(Name.c_str(), 0, tile))
	{
		RenderSize = Gdiplus::Size(tile->Image->GetWidth() / 2, tile->Image->GetHeight() / 2);
		Thumbnail = tile->Image->Clone(Gdiplus::Rect(0, 0, tile->Image->GetWidth(), tile->Image->GetHeight()), PixelFormatDontCare);
		OverlapBounds = Gdiplus::Rect(Gdiplus::Point(0, 0), Gdiplus::Size((RenderSize.Width + TileWidth - 1) / TileWidth, (RenderSize.Height + TileHeight - 1) / TileHeight));
	}
}
