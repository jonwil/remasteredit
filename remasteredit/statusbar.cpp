#include "global.h"
#include <CommCtrl.h>
#include <windowsx.h>
#include "statusbar.h"

StatusBar::StatusBar(int count) : hWndStatusBar(nullptr), Count(count)
{
	strings = new std::string[count];
}

void StatusBar::Create(HINSTANCE instance, HWND parent)
{
	hWndStatusBar = CreateWindowEx(0, STATUSCLASSNAME, (LPSTR)NULL, WS_CHILD | WS_VISIBLE /*| TBSTYLE_TOOLTIPS*/, 0, 0, 0, 0, parent, nullptr, instance, NULL);
}

void StatusBar::OnSize()
{
	int* pos = new int[Count];
	HDC hDC = GetDC(hWndStatusBar);
	for (int i = 0; i < Count; i++)
	{
		RECT r;
		r.left = 0;
		r.right = 0;
		r.top = 0;
		r.bottom = 0;
		DrawText(hDC, strings[i].c_str(), strlen(strings[i].c_str()), &r, DT_CALCRECT);
		pos[i] = r.right;
	}
	ReleaseDC(hWndStatusBar, hDC);
	SendMessage(hWndStatusBar, SB_SETPARTS, Count, reinterpret_cast<LPARAM>(pos));
	for (int i = 0; i < Count; i++)
	{
		SendMessage(hWndStatusBar, SB_SETTEXT, i, reinterpret_cast<LPARAM>(strings[i].c_str()));
	}
	delete[] pos;
}

void StatusBar::SetText(int pos, const char* str)
{
	if (pos <= Count)
	{
		strings[pos] = str;
		OnSize();
	}
}