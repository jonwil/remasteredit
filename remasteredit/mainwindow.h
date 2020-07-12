#pragma once
#include "window.h"
#include "resource.h"
class MainWindow : public Window
{
public:
	MainWindow(HINSTANCE hInstance)
	{
		mainwindow = this;
		Create(WS_EX_CONTROLPARENT, "MainWindow", "Remaster Map Editor", WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MAINMENU)), hInstance);
	}
	~MainWindow()
	{
		mainwindow = nullptr;
	}
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	MainWindow* mainwindow;
};