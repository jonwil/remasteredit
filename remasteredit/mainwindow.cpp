#include "global.h"
#include "mainwindow.h"
#include "mappanel.h"
#include <commdlg.h>
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
				WORD cx = LOWORD(lParam);
				WORD cy = HIWORD(lParam);
				SetWindowPos(MapPanel::panel->window, nullptr, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
				InvalidateRect(MapPanel::panel->window, nullptr, TRUE);
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
