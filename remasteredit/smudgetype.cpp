#include "smudgetype.h"
std::map<std::string, SmudgeType*> SmudgeType::SmudgeMapTD;
std::map<std::string, SmudgeType*> SmudgeType::SmudgeMapRA;
static SmudgeType CRATER1TD(SMUDGETD_CRATER1, "cr1", 1, 1, SMUDGETYPE_NONE, false);
static SmudgeType CRATER2TD(SMUDGETD_CRATER2, "cr2", 1, 1, SMUDGETYPE_NONE, false);
static SmudgeType CRATER3TD(SMUDGETD_CRATER3, "cr3", 1, 1, SMUDGETYPE_NONE, false);
static SmudgeType CRATER4TD(SMUDGETD_CRATER4, "cr4", 1, 1, SMUDGETYPE_NONE, false);
static SmudgeType CRATER5TD(SMUDGETD_CRATER5, "cr5", 1, 1, SMUDGETYPE_NONE, false);
static SmudgeType CRATER6TD(SMUDGETD_CRATER6, "cr6", 1, 1, SMUDGETYPE_NONE, false);
static SmudgeType SCORCH1TD(SMUDGETD_SCORCH1, "sc1", 1, 1, SMUDGETYPE_NONE, false);
static SmudgeType SCORCH2TD(SMUDGETD_SCORCH2, "sc2", 1, 1, SMUDGETYPE_NONE, false);
static SmudgeType SCORCH3TD(SMUDGETD_SCORCH3, "sc3", 1, 1, SMUDGETYPE_NONE, false);
static SmudgeType SCORCH4TD(SMUDGETD_SCORCH4, "sc4", 1, 1, SMUDGETYPE_NONE, false);
static SmudgeType SCORCH5TD(SMUDGETD_SCORCH5, "sc5", 1, 1, SMUDGETYPE_NONE, false);
static SmudgeType SCORCH6TD(SMUDGETD_SCORCH6, "sc6", 1, 1, SMUDGETYPE_NONE, false);
static SmudgeType BIB1TD(SMUDGETD_BIB1, "bib1", 4, 2, SMUDGETYPE_BIB, false);
static SmudgeType BIB2TD(SMUDGETD_BIB2, "bib2", 3, 2, SMUDGETYPE_BIB, false);
static SmudgeType BIB3TD(SMUDGETD_BIB3, "bib3", 2, 2, SMUDGETYPE_BIB, false);
static SmudgeType CRATER1RA(SMUDGERA_CRATER1, "cr1", 1, 1, SMUDGETYPE_NONE, true);
static SmudgeType CRATER2RA(SMUDGERA_CRATER2, "cr2", 1, 1, SMUDGETYPE_NONE, true);
static SmudgeType CRATER3RA(SMUDGERA_CRATER3, "cr3", 1, 1, SMUDGETYPE_NONE, true);
static SmudgeType CRATER4RA(SMUDGERA_CRATER4, "cr4", 1, 1, SMUDGETYPE_NONE, true);
static SmudgeType CRATER5RA(SMUDGERA_CRATER5, "cr5", 1, 1, SMUDGETYPE_NONE, true);
static SmudgeType CRATER6RA(SMUDGERA_CRATER6, "cr6", 1, 1, SMUDGETYPE_NONE, true);
static SmudgeType SCORCH1RA(SMUDGERA_SCORCH1, "sc1", 1, 1, SMUDGETYPE_NONE, true);
static SmudgeType SCORCH2RA(SMUDGERA_SCORCH2, "sc2", 1, 1, SMUDGETYPE_NONE, true);
static SmudgeType SCORCH3RA(SMUDGERA_SCORCH3, "sc3", 1, 1, SMUDGETYPE_NONE, true);
static SmudgeType SCORCH4RA(SMUDGERA_SCORCH4, "sc4", 1, 1, SMUDGETYPE_NONE, true);
static SmudgeType SCORCH5RA(SMUDGERA_SCORCH5, "sc5", 1, 1, SMUDGETYPE_NONE, true);
static SmudgeType SCORCH6RA(SMUDGERA_SCORCH6, "sc6", 1, 1, SMUDGETYPE_NONE, true);
static SmudgeType BIB1RA(SMUDGERA_BIB1, "bib1", 4, 2, SMUDGETYPE_BIB, true);
static SmudgeType BIB2RA(SMUDGERA_BIB2, "bib2", 3, 2, SMUDGETYPE_BIB, true);
static SmudgeType BIB3RA(SMUDGERA_BIB3, "bib3", 2, 2, SMUDGETYPE_BIB, true);
SmudgeType const* const SmudgeType::PointersTD[SMUDGETD_COUNT] = {
	& CRATER1TD,
	& CRATER2TD,
	& CRATER3TD,
	& CRATER4TD,
	& CRATER5TD,
	& CRATER6TD,
	& SCORCH1TD,
	& SCORCH2TD,
	& SCORCH3TD,
	& SCORCH4TD,
	& SCORCH5TD,
	& SCORCH6TD,
	& BIB1TD,
	& BIB2TD,
	& BIB3TD,
};
SmudgeType const* const SmudgeType::PointersRA[SMUDGERA_COUNT] = {
	& CRATER1RA,
	& CRATER2RA,
	& CRATER3RA,
	& CRATER4RA,
	& CRATER5RA,
	& CRATER6RA,
	& SCORCH1RA,
	& SCORCH2RA,
	& SCORCH3RA,
	& SCORCH4RA,
	& SCORCH5RA,
	& SCORCH6RA,
	& BIB1RA,
	& BIB2RA,
	& BIB3RA,
};