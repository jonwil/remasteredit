#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdlib.h>
#include <Shlwapi.h>
#pragma warning(disable: 4458)
#include <gdiplus.h>
#pragma warning(default: 4458)
#define FREEIMAGE_LIB
#include "FreeImage.h"
#include "megfilemanager.h"
#include "TemplateType.h"
#include "tileset.h"
#include "map.h"
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

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);
    if (__argc < 2)
    {
        return 0;
    }
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::Status status = Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	if (status != Gdiplus::Ok)
	{
		return 0;
	}
	LoadMegs(__argv[1]);
	InitTilesets();
	FreeImage_Initialise();
	Map* map = new Map();
	map->Load(__argv[2]);
	BYTE* data = new BYTE[map->tilesize * map->width * map->tilesize * map->height * 4];
	Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(map->tilesize * map->width, map->tilesize * map->height, map->tilesize * map->width * 4, PixelFormat32bppARGB, data);
	Gdiplus::Graphics* graphics = Gdiplus::Graphics::FromImage(bitmap);
	map->Render(graphics);
	CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);
	bitmap->Save(L"test.png", &pngClsid);
	delete[] data;
	delete graphics;
	delete bitmap;
	FreeImage_DeInitialise();
	ShutdownTilesets();
	CloseMegs();
	delete map;
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return 0;
}
