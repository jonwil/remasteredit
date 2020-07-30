#pragma once
#include "window.h"
#include "resource.h"
#include "toolbar.h"
#include "statusbar.h"
class Tool;
enum ToolType
{
	TOOL_NONE = 0,
	TOOL_MAP = 1,
	TOOL_SMUDGE = 2,
	TOOL_OVERLAY = 4,
	TOOL_TERRAIN = 8,
	TOOL_INFANTRY = 16,
	TOOL_UNIT = 32,
	TOOL_AIRCRAFT = 64,
	TOOL_VESSEL = 128,
	TOOL_BUILDING = 256,
	TOOL_RESOURCES = 512,
	TOOL_WALL = 1024,
	TOOL_WAYPOINT = 2048,
	TOOL_CELLTRIGGER = 4096
};
class MainWindow : public Window
{
public:
	MainWindow(HINSTANCE hInstance)
	{
		instance = hInstance;
		mainwindow = this;
		activeToolType = TOOL_NONE;
		availableToolTypes = TOOL_NONE;
		activeTool = nullptr;
		activeWindow = nullptr;
		activeLayers = MAPLAYER_NONE;
		Create(WS_EX_CONTROLPARENT, "MainWindow", "Remaster Map Editor", WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MAINMENU)), hInstance);
		toolbar = new Toolbar();
		toolbar->Create(hInstance, window, IDR_TOOLBAR);
		RECT r;
		GetClientRect(toolbar->hWndToolBar, &r);
		barheight = r.bottom - r.top;
		statusbar = new StatusBar(2);
		statusbar->Create(hInstance, window);
		RECT r2;
		GetClientRect(statusbar->hWndStatusBar, &r2);
		sbheight = r2.bottom - r2.top;
	}
	~MainWindow()
	{
		mainwindow = nullptr;
		delete toolbar;
	}
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	void RefreshAvailableTools();
	void SetActiveTool(int toolType);
	static MainWindow* mainwindow;
	Toolbar* toolbar;
	StatusBar* statusbar;
	int barheight;
	int sbheight;
	int availableToolTypes;
	int activeToolType;
	Tool* activeTool;
	Window* activeWindow;
	MapLayerFlag activeLayers;
	HINSTANCE instance;
	void RefreshActiveTool();
	void ClearActiveTool();
};
