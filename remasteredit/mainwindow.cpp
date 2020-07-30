#include "global.h"
#include "mainwindow.h"
#include "mappanel.h"
#include <commdlg.h>
#include "TemplateType.h"
#include "smudgetype.h"
#include "overlaytype.h"
#include "terraintype.h"
#include "infantrytype.h"
#include "structtype.h"
#include "unittype.h"
#include "vesseltype.h"
#include "AircraftType.h"
#include "map.h"
#include "tools.h"
MainWindow* MainWindow::mainwindow;
bool GetOpenFile(char* buf, const char* filter, const char* dir, HWND parent, const char* title)
{
	OPENFILENAME of;

	memset(&of, 0, sizeof(OPENFILENAME));

	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = parent;
	of.hInstance = nullptr;
	of.lpstrFilter = filter;
	of.lpstrCustomFilter = nullptr;
	of.nMaxCustFilter = 0;
	of.nFilterIndex = 0;
	of.lpstrFile = buf;
	of.nMaxFile = MAX_PATH;
	of.lpstrFileTitle = nullptr;
	of.nMaxFileTitle = 0;
	of.lpstrInitialDir = dir;
	of.lpstrTitle = title;
	of.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER;
	of.nFileOffset = 0;
	of.nFileExtension = 0;
	of.lpstrDefExt = nullptr;
	of.lCustData = 0;
	of.lpfnHook = nullptr;
	of.lpTemplateName = nullptr;

	if (!GetOpenFileName(&of)) return false;
	return true;
}
LRESULT MainWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_OPEN:
			{
				char fname[MAX_PATH];
				fname[0] = 0;
				if (GetOpenFile(fname, "Maps (*.mpr;*.ini)\0*.mpr;*.ini\0\0", nullptr, hWnd, "Open Map"))
				{
					MainPanel->FileOpen(fname);
					ClearActiveTool();
					RefreshAvailableTools();
					RefreshActiveTool();
				}
			}
			break;
		case ID_FILE_EXIT:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		case ID_MAP_TOOL:
			SetActiveTool(TOOL_MAP);
			break;
		case ID_SMUDGE_TOOL:
			SetActiveTool(TOOL_SMUDGE);
			break;
		case ID_OVERLAY_TOOL:
			SetActiveTool(TOOL_OVERLAY);
			break;
		case ID_TERRAIN_TOOL:
			SetActiveTool(TOOL_TERRAIN);
			break;
		case ID_INFANTRY_TOOL:
			SetActiveTool(TOOL_INFANTRY);
			break;
		case ID_UNIT_TOOL:
			SetActiveTool(TOOL_UNIT);
			break;
		case ID_AIRCRAFT_TOOL:
			SetActiveTool(TOOL_AIRCRAFT);
			break;
		case ID_VESSEL_TOOL:
			SetActiveTool(TOOL_VESSEL);
			break;
		case ID_STRUCTURE_TOOL:
			SetActiveTool(TOOL_BUILDING);
			break;
		case ID_RESOURCES_TOOL:
			SetActiveTool(TOOL_RESOURCES);
			break;
		case ID_WALLS_TOOL:
			SetActiveTool(TOOL_WALL);
			break;
		case ID_WAYPOINTS_TOOL:
			SetActiveTool(TOOL_WAYPOINT);
			break;
		case ID_CELL_TRIGGERS_TOOL:
			SetActiveTool(TOOL_CELLTRIGGER);
			break;
		}
		break;
	case WM_SIZE:
		{
			if (MainPanel)
			{
				if (toolbar->hWndToolBar && statusbar->hWndStatusBar)
				{
					WORD cx = LOWORD(lParam);
					WORD cy = HIWORD(lParam);
					RECT r;
					GetClientRect(toolbar->hWndToolBar, &r);
					barheight = r.bottom - r.top;
					RECT r2;
					GetClientRect(statusbar->hWndStatusBar, &r2);
					sbheight = r2.bottom - r2.top;
					int clientwidth = cx;
					int clientheight = cy - barheight - sbheight;
					SetWindowPos(MainPanel->window, nullptr, 0, barheight, clientwidth, clientheight, SWP_NOZORDER);
					InvalidateRect(MainPanel->window, nullptr, TRUE);
					toolbar->OnSize();
					statusbar->OnSize();
				}
			}
		}
	return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void MainWindow::RefreshAvailableTools()
{
	Map* map = MainPanel->map;
	availableToolTypes = TOOL_NONE;
	if (map)
	{
		availableToolTypes |= TOOL_WAYPOINT;
		if (map->isra)
		{
			if constexpr (TEMPLATERA_COUNT != 0)
			{
				availableToolTypes |= TOOL_MAP;
			}
			if (Any(SmudgeType::SmudgeMapRA))
			{
				availableToolTypes |= TOOL_SMUDGE;
			}
			if (Any(OverlayType::OverlayMapRA, [map](const std::pair<std::string, OverlayType*> pair) { return pair.second->IsPlaceable && (pair.second->Theater & map->theaterid); }))
			{
				availableToolTypes |= TOOL_OVERLAY;
			}
			if (Any(TerrainType::TerrainMapRA, [map](const std::pair<std::string, TerrainType*> pair) { return pair.second->Theater & map->theaterid; }))
			{
				availableToolTypes |= TOOL_TERRAIN;
			}
			if (Any(InfantryType::InfantryMapRA))
			{
				availableToolTypes |= TOOL_INFANTRY;
			}
			if (Any(UnitType::UnitMapRA))
			{
				availableToolTypes |= TOOL_UNIT;
			}
			if (Any(AircraftType::AircraftMapRA))
			{
				availableToolTypes |= TOOL_AIRCRAFT;
			}
			if (Any(VesselType::VesselMapRA))
			{
				availableToolTypes |= TOOL_VESSEL;
			}
			if (Any(StructType::StructMapRA))
			{
				availableToolTypes |= TOOL_BUILDING;
			}
			if (Any(OverlayType::OverlayMapRA, [map](const std::pair<std::string, OverlayType*> pair) { return pair.second->IsResource; }))
			{
				availableToolTypes |= TOOL_RESOURCES;
			}
			if (Any(OverlayType::OverlayMapRA, [map](const std::pair<std::string, OverlayType*> pair) { return pair.second->IsWall; }))
			{
				availableToolTypes |= TOOL_WALL;
			}
			//todo triggers
		}
		else
		{
			if constexpr (TEMPLATETD_COUNT != 0)
			{
				availableToolTypes |= TOOL_MAP;
			}
			if (Any(SmudgeType::SmudgeMapTD))
			{
				availableToolTypes |= TOOL_SMUDGE;
			}
			if (Any(OverlayType::OverlayMapTD, [map](const std::pair<std::string, OverlayType*> pair) { return pair.second->IsPlaceable && (pair.second->Theater & map->theaterid); }))
			{
				availableToolTypes |= TOOL_OVERLAY;
			}
			if (Any(TerrainType::TerrainMapTD, [map](const std::pair<std::string, TerrainType*> pair) { return pair.second->Theater & map->theaterid; }))
			{
				availableToolTypes |= TOOL_TERRAIN;
			}
			if (Any(InfantryType::InfantryMapTD))
			{
				availableToolTypes |= TOOL_INFANTRY;
			}
			if (Any(UnitType::UnitMapTD))
			{
				availableToolTypes |= TOOL_UNIT;
			}
			if (Any(AircraftType::AircraftMapTD))
			{
				availableToolTypes |= TOOL_AIRCRAFT;
			}
			if (Any(StructType::StructMapTD))
			{
				availableToolTypes |= TOOL_BUILDING;
			}
			if (Any(OverlayType::OverlayMapTD, [map](const std::pair<std::string, OverlayType*> pair) { return pair.second->IsResource; }))
			{
				availableToolTypes |= TOOL_RESOURCES;
			}
			if (Any(OverlayType::OverlayMapTD, [map](const std::pair<std::string, OverlayType*> pair) { return pair.second->IsWall; }))
			{
				availableToolTypes |= TOOL_WALL;
			}
			//todo triggers
		}
	}
	toolbar->Enable(ID_MAP_TOOL, (availableToolTypes & TOOL_MAP));
	toolbar->Enable(ID_SMUDGE_TOOL, (availableToolTypes & TOOL_SMUDGE));
	toolbar->Enable(ID_OVERLAY_TOOL, (availableToolTypes & TOOL_OVERLAY));
	toolbar->Enable(ID_TERRAIN_TOOL, (availableToolTypes & TOOL_TERRAIN));
	toolbar->Enable(ID_INFANTRY_TOOL, (availableToolTypes & TOOL_INFANTRY));
	toolbar->Enable(ID_UNIT_TOOL, (availableToolTypes & TOOL_UNIT));
	toolbar->Enable(ID_AIRCRAFT_TOOL, (availableToolTypes & TOOL_AIRCRAFT));
	toolbar->Enable(ID_VESSEL_TOOL, (availableToolTypes & TOOL_VESSEL));
	toolbar->Enable(ID_STRUCTURE_TOOL, (availableToolTypes & TOOL_BUILDING));
	toolbar->Enable(ID_RESOURCES_TOOL, (availableToolTypes & TOOL_RESOURCES));
	toolbar->Enable(ID_WALLS_TOOL, (availableToolTypes & TOOL_WALL));
	toolbar->Enable(ID_WAYPOINTS_TOOL, (availableToolTypes & TOOL_WAYPOINT));
	toolbar->Enable(ID_CELL_TRIGGERS_TOOL, (availableToolTypes & TOOL_CELLTRIGGER));
	SetActiveTool(activeToolType);
}
ToolType toolTypes[13] = { TOOL_MAP, TOOL_SMUDGE, TOOL_OVERLAY, TOOL_TERRAIN, TOOL_INFANTRY, TOOL_UNIT, TOOL_AIRCRAFT, TOOL_VESSEL, TOOL_BUILDING, TOOL_RESOURCES, TOOL_WALL, TOOL_WAYPOINT, TOOL_CELLTRIGGER };
void MainWindow::SetActiveTool(int toolType)
{
	if ((availableToolTypes & toolType) == 0)
	{
		auto source = from(toolTypes).where([&](ToolType t) {return (availableToolTypes & t) != 0; }).toStdVector();
		toolType = Any(source) ? *source.begin() : TOOL_NONE;
	}
	if (activeToolType != toolType)
	{
		activeToolType = toolType;
		RefreshActiveTool();
	}
}

void MainWindow::RefreshActiveTool()
{
	if (MainPanel->map)
	{
		if (activeTool == nullptr)
		{
			activeLayers = MAPLAYER_NONE;
		}
		ClearActiveTool();
		switch (activeToolType)
		{
			case TOOL_MAP:
			{
				TemplateDialog* dialog = new TemplateDialog(24, 120, 290, 729, window, instance);
				activeTool = new TemplateTool(MainPanel->map, MainPanel, activeLayers, dialog->TemplateTypeListView, dialog->TemplateTypeMapPanel);
				activeWindow = dialog;
				ShowWindow(activeWindow->window, SW_SHOW);
			}
			break;
		}
		MainPanel->currentTool = activeTool;
		if (MainPanel->map->isra)
		{
			MainPanel->SetMaxZoom(16);
			MainPanel->SetZoomStep(2);
		}
		else
		{
			MainPanel->SetMaxZoom(8);
			MainPanel->SetZoomStep(1);
		}
		toolbar->Check(ID_MAP_TOOL, (activeToolType == TOOL_MAP));
		toolbar->Check(ID_SMUDGE_TOOL, (activeToolType == TOOL_SMUDGE));
		toolbar->Check(ID_OVERLAY_TOOL, (activeToolType == TOOL_OVERLAY));
		toolbar->Check(ID_TERRAIN_TOOL, (activeToolType == TOOL_TERRAIN));
		toolbar->Check(ID_INFANTRY_TOOL, (activeToolType == TOOL_INFANTRY));
		toolbar->Check(ID_UNIT_TOOL, (activeToolType == TOOL_UNIT));
		toolbar->Check(ID_AIRCRAFT_TOOL, (activeToolType == TOOL_AIRCRAFT));
		toolbar->Check(ID_VESSEL_TOOL, (activeToolType == TOOL_VESSEL));
		toolbar->Check(ID_STRUCTURE_TOOL, (activeToolType == TOOL_BUILDING));
		toolbar->Check(ID_RESOURCES_TOOL, (activeToolType == TOOL_RESOURCES));
		toolbar->Check(ID_WALLS_TOOL, (activeToolType == TOOL_WALL));
		toolbar->Check(ID_WAYPOINTS_TOOL, (activeToolType == TOOL_WAYPOINT));
		toolbar->Check(ID_CELL_TRIGGERS_TOOL, (activeToolType == TOOL_CELLTRIGGER));
		activeLayers = MAPLAYER_ALL;
		MainPanel->Invalidate();
	}
}

void MainWindow::ClearActiveTool()
{
	if (activeTool)
	{
		delete activeTool;
		activeTool = nullptr;
	}
	if (activeWindow)
	{
		delete activeWindow;
		activeWindow = nullptr;
		statusbar->SetText(0, "");
	}
}