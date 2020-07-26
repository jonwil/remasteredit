#pragma once
#include "window.h"
#include "resource.h"
#include "toolbar.h"
#include "statusbar.h"
class MainWindow : public Window
{
public:
	MainWindow(HINSTANCE hInstance)
	{
		mainwindow = this;
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
	static MainWindow* mainwindow;
	Toolbar* toolbar;
	StatusBar* statusbar;
	int barheight;
	int sbheight;
};