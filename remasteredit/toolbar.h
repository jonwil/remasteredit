#pragma once
#define RT_TOOLBAR MAKEINTRESOURCE(241)
#define ID_TOOLBAR 0xE800
struct TOOLBARDATA
{
	WORD wVersion;
	WORD wWidth;
	WORD wHeight;
	WORD wItemCount;
	WORD items[1];
};

class Toolbar
{
public:
	Toolbar();
	void Create(HINSTANCE instance, HWND parent, int id);
	void OnSize();
	void Enable(int id, bool enabled);
	void Check(int id, bool checked);
	HWND hWndToolBar;
};