#include "global.h"
#include "global.h"
#include <stdlib.h>
#define FREEIMAGE_LIB
#include "FreeImage.h"
#include "megfilemanager.h"
#include "TemplateType.h"
#include "tileset.h"
#include "map.h"
#include "teamcolor.h"
#include "texman.h"
#include "resource.h"
#include "mainwindow.h"
#include "mappanel.h"
#include "textman.h"
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

/*void PreRender(Gdiplus::Graphics* g, std::vector<Gdiplus::Point> cells)
{
}
*/

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::Status status = Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	if (status != Gdiplus::Ok)
	{
		return 0;
	}
	if (__argc > 1)
	{
		LoadMegs(__argv[1]);
	}
	else
	{
		char path[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, path);
		LoadMegs(path);
	}
	InitTeamColors();
	InitTextures();
	InitTilesets();
	InitText();
	FreeImage_Initialise();
	MainWindow wnd(hInstance);
	RECT r;
	GetClientRect(wnd.window, &r);
	int width = r.right - r.left;
	int height = r.bottom - r.top;
	height -= wnd.barheight;
	height -= wnd.sbheight;
	MapPanel panel(hInstance,0,wnd.barheight,width,height,wnd.window);
	ShowWindow(wnd.window, SW_MAXIMIZE);
	UpdateWindow(wnd.window);
	UpdateWindow(panel.window);
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	FreeImage_DeInitialise();
	ShutdownText();
	ShutdownTilesets();
	ShutdownTextures();
	ShutdownTeamColors();
	CloseMegs();
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return 0;
}
