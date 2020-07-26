#include "global.h"
#include <CommCtrl.h>
#include <windowsx.h>
#include "toolbar.h"

Toolbar::Toolbar() : hWndToolBar(nullptr)
{
}

void Toolbar::Create(HINSTANCE instance, HWND parent, int id)
{
	hWndToolBar = CreateWindowEx(0, TOOLBARCLASSNAME, nullptr, WS_CHILD | WS_VISIBLE /*| TBSTYLE_TOOLTIPS*/, 0, 0, 0, 0, parent, reinterpret_cast<HMENU>(ID_TOOLBAR), instance, NULL);
	SetWindowLong(hWndToolBar, GWL_STYLE, GetWindowStyle(hWndToolBar) | TBSTYLE_FLAT);
	SendMessage(hWndToolBar, TB_BUTTONSTRUCTSIZE, static_cast<WPARAM>(sizeof(TBBUTTON)), 0);
	HRSRC hRsrc = FindResource(instance, MAKEINTRESOURCE(id), RT_TOOLBAR);
	HGLOBAL hGlobal = LoadResource(instance, hRsrc);
	TOOLBARDATA* pData = static_cast<TOOLBARDATA*>(LockResource(hGlobal));
	UINT* pItems = new UINT[pData->wItemCount];
	for (int i = 0; i < pData->wItemCount; i++)
		pItems[i] = pData->items[i];
	auto lpIDArray = pItems;
	auto nIDCount = pData->wItemCount;
	int nCount = static_cast<int>(SendMessage(hWndToolBar, TB_BUTTONCOUNT, 0, 0));
	while (nCount--)
	{
		SendMessage(hWndToolBar, TB_DELETEBUTTON, 0, 0);
	}
	SendMessage(hWndToolBar, TB_SETBITMAPSIZE, 0, MAKELONG(pData->wWidth, pData->wHeight));
	SendMessage(hWndToolBar, TB_SETBUTTONSIZE, 0, MAKELONG(pData->wWidth + 7, pData->wHeight + 7));
	UnlockResource(hGlobal);
	FreeResource(hGlobal);
	SendMessage(hWndToolBar, TB_SETBITMAPSIZE, 0, MAKELONG(16, 15));
	HBITMAP hbmImageWell = LoadBitmap(instance, MAKEINTRESOURCE(id));
	BITMAP bitmap;
	GetObject(hbmImageWell, sizeof(BITMAP), &bitmap);
	TBADDBITMAP tbab;
	tbab.hInst = instance;
	tbab.nID = id;
	SendMessage(hWndToolBar, TB_ADDBITMAP, bitmap.bmWidth / pData->wWidth, reinterpret_cast<LPARAM>(&tbab));
	DeleteObject(hbmImageWell);
	TBBUTTON button; memset(&button, 0, sizeof(TBBUTTON));
	button.iString = -1;
	if (lpIDArray != NULL)
	{
		int iImage = 0;
		for (int i = 0; i < nIDCount; i++)
		{
			button.fsState = TBSTATE_ENABLED;
			if ((button.idCommand = *lpIDArray++) == 0)
			{
				button.fsStyle = BTNS_SEP;
				button.iBitmap = 6;
			}
			else
			{
				button.fsStyle = BTNS_BUTTON;
				button.iBitmap = iImage++;
			}
			SendMessage(hWndToolBar, TB_ADDBUTTONS, 1, reinterpret_cast<LPARAM>(&button));
		}
	}
	delete[] pItems;
}

void Toolbar::OnSize()
{
	SendMessage(hWndToolBar, TB_AUTOSIZE, 0, 0);
}

void Toolbar::Enable(int id, bool enabled)
{
	WPARAM state = SendMessage(hWndToolBar, TB_GETSTATE, id, 0);
	if (enabled)
	{
		state |= TBSTATE_ENABLED;
	}
	else
	{
		state &= ~TBSTATE_ENABLED;
	}
	SendMessage(hWndToolBar, TB_SETSTATE, id, state);
}

void Toolbar::Check(int id, bool checked)
{
	TBBUTTONINFO info;
	info.cbSize = sizeof(info);
	info.dwMask = TBIF_STATE | TBIF_STYLE;
	SendMessage(hWndToolBar, TB_GETBUTTONINFO, id, reinterpret_cast<LPARAM>(&info));
	info.fsStyle |= BTNS_CHECK;
	if (checked)
	{
		info.fsState |= TBSTATE_CHECKED;
	}
	else
	{
		info.fsState &= ~TBSTATE_CHECKED;
	}
	SendMessage(hWndToolBar, TB_SETBUTTONINFO, id, reinterpret_cast<LPARAM>(&info));
}
