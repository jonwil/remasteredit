#include "global.h"
#include "mainwindow.h"
#include "mappanel.h"
#include <commdlg.h>
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
					MapPanel::panel->FileOpen(fname);
				}
			}
			break;
		case ID_FILE_EXIT:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		break;
	case WM_SIZE:
		{
			if (MapPanel::panel)
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
					SetWindowPos(MapPanel::panel->window, nullptr, 0, barheight, clientwidth, clientheight, SWP_NOZORDER);
					InvalidateRect(MapPanel::panel->window, nullptr, TRUE);
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
