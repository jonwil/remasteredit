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
#include "teamcolor.h"
#include "texman.h"
#include "resource.h"
#include <commdlg.h>
#include <windowsx.h>
void UpdateCamera();
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

HWND MainWnd;
HWND ScrollWnd;
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
Map* map = nullptr;
BYTE* data = nullptr;
Gdiplus::Bitmap* mapImage = nullptr;
Gdiplus::Graphics* graphics = nullptr;
bool updatingCamera = false;
Gdiplus::Rect cameraBounds;
Gdiplus::Point lastScrollPosition(0,0);
Gdiplus::Matrix *mapToViewTransform;
Gdiplus::Matrix *viewToPageTransform;
Gdiplus::Matrix *compositeTransform;
Gdiplus::Matrix *invCompositeTransform;
bool referenceset = false;
Gdiplus::Point referencePositionsMap;
Gdiplus::SizeF referencePositionsClient;
bool fullInvalidation = false;
int minZoom = 1;
int maxZoom = 8;
int zoomStep = 1;
int zoom = 1;
int clientWidth = 0;
int clientHeight = 0;
Gdiplus::Size AutoScrollMinSize;
Gdiplus::Size lastScrollMinSize;
Gdiplus::Point AutoScrollPosition(0,0);
Gdiplus::Point MapToClient(Gdiplus::Point point)
{
	Gdiplus::Point pt = point;
	compositeTransform->TransformPoints(&pt);
	return pt;
}

Gdiplus::Size MapToClient(Gdiplus::Size size)
{
	Gdiplus::Point pt = size;
	Gdiplus::REAL m[6];
	compositeTransform->GetElements(m);
	compositeTransform->TransformVectors(&pt);
	return Gdiplus::Size(pt.X,pt.Y);
}

Gdiplus::Rect MapToClient(Gdiplus::Rect rectangle)
{
	Gdiplus::Point pt[2];
	rectangle.GetLocation(&pt[0]);
	pt[1] = Gdiplus::Point(rectangle.GetRight(), rectangle.GetBottom());
	compositeTransform->TransformPoints(pt, 2);
	return Gdiplus::Rect(pt[0], Gdiplus::Size(pt[1].X - pt[0].X, pt[1].Y - pt[0].Y));
}

Gdiplus::Point ClientToMap(Gdiplus::Point point)
{
	Gdiplus::Point pt = point;
	invCompositeTransform->TransformPoints(&pt);
	return pt;
}

Gdiplus::Point ClientToMap(Gdiplus::Size size)
{
	Gdiplus::Point pt = size;
	invCompositeTransform->TransformVectors(&pt);
	return pt;
}

Gdiplus::Rect ClientToMap(Gdiplus::Rect rectangle)
{
	Gdiplus::Point pt[2];
	rectangle.GetLocation(&pt[0]);
	pt[1] = Gdiplus::Point(rectangle.GetRight(), rectangle.GetBottom());
	invCompositeTransform->TransformPoints(pt, 2);
	return Gdiplus::Rect(pt[0], Gdiplus::Size(pt[1].X - pt[0].X, pt[1].Y - pt[0].Y));
}

void RecalculateTransforms()
{
	mapToViewTransform->Reset();
	mapToViewTransform->Translate((float)cameraBounds.GetLeft(), (float)cameraBounds.GetTop());
	mapToViewTransform->Scale((float)cameraBounds.Width, (float)cameraBounds.Height);
	mapToViewTransform->Invert();
	Gdiplus::REAL m[6];
	mapToViewTransform->GetElements(m);
	viewToPageTransform->Reset();
	viewToPageTransform->Scale((float)clientWidth, (float)clientHeight);
	viewToPageTransform->GetElements(m);
	compositeTransform->Reset();
	compositeTransform->Multiply(viewToPageTransform);
	compositeTransform->Multiply(mapToViewTransform);
	compositeTransform->GetElements(m);
	invCompositeTransform->Reset();
	invCompositeTransform->Multiply(compositeTransform);
	invCompositeTransform->Invert();
	invCompositeTransform->GetElements(m);
}

bool hscroll = false;
bool vscroll = false;
void UpdateScrollBars()
{
	bool needhscroll = false;
	bool needvscroll = false;
	Gdiplus::Rect currentClient(0, 0, clientWidth, clientHeight);
	Gdiplus::Rect fullClient = currentClient;
	Gdiplus::Rect minClient = currentClient;
	if (hscroll)
	{
		fullClient.Height += GetSystemMetrics(SM_CXHSCROLL);
	}
	else
	{
		minClient.Height -= GetSystemMetrics(SM_CXHSCROLL);
	}
	if (vscroll)
	{
		fullClient.Width += GetSystemMetrics(SM_CYVSCROLL);
	}
	else
	{
		minClient.Width -= GetSystemMetrics(SM_CYVSCROLL);
	}
	int maxX = minClient.Width;
	int maxY = minClient.Height;
	if (AutoScrollMinSize.Width || AutoScrollMinSize.Height)
	{
		maxX = AutoScrollMinSize.Width;
		maxY = AutoScrollMinSize.Height;
		needhscroll = true;
		needvscroll = true;
	}
	if (maxX <= fullClient.Width)
	{
		needhscroll = false;
	}
	if (maxY <= fullClient.Height)
	{
		needvscroll = false;
	}
	Gdiplus::Rect clientToBe = fullClient;
	if (needhscroll)
	{
		clientToBe.Height -= GetSystemMetrics(SM_CXHSCROLL);
	}
	if (needvscroll)
	{
		clientToBe.Width -= GetSystemMetrics(SM_CYVSCROLL);
	}
	if (needhscroll && maxY > clientToBe.Height)
	{
		needvscroll = true;
	}
	if (needvscroll && maxX > clientToBe.Width)
	{
		needhscroll = true;
	}
	if (!needhscroll)
	{
		maxX = clientToBe.Width;
	}
	if (!needvscroll)
	{
		maxY = clientToBe.Height;
	}
	bool scroll = false;
	if (needhscroll != hscroll || needvscroll != vscroll)
	{
		scroll = true;
	}
	if (clientWidth != clientToBe.Width || clientHeight != clientToBe.Height)
	{
		scroll = true;
	}
	if (lastScrollMinSize.Width != AutoScrollMinSize.Width || lastScrollMinSize.Height != AutoScrollMinSize.Height)
	{
		scroll = true;
	}
	clientWidth = clientToBe.Width;
	clientHeight = clientToBe.Height;
	if (scroll)
	{
		hscroll = needhscroll;
		vscroll = needvscroll;
		ShowScrollBar(ScrollWnd, SB_HORZ, needhscroll);
		ShowScrollBar(ScrollWnd, SB_VERT, needvscroll);
		SetWindowPos(ScrollWnd, nullptr, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
		if (needhscroll)
		{
			SCROLLINFO si;
			si.cbSize = sizeof(si);
			si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
			si.nMin = 0;
			si.nMax = AutoScrollMinSize.Width - 1;
			si.nPage = clientWidth;
			si.nPos = -AutoScrollPosition.X;
			si.nTrackPos = 0;
			SetScrollInfo(ScrollWnd, SB_HORZ, &si, TRUE);
		}
		if (needvscroll)
		{
			SCROLLINFO si;
			si.cbSize = sizeof(si);
			si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
			si.nMin = 0;
			si.nMax = AutoScrollMinSize.Height - 1;
			si.nPage = clientHeight;
			si.nPos = -AutoScrollPosition.Y;
			si.nTrackPos = 0;
			SetScrollInfo(ScrollWnd, SB_VERT, &si, TRUE);
		}
		UpdateCamera();
	}
}

void SuspendDrawing()
{
	SendMessage(ScrollWnd, WM_SETREDRAW, FALSE, 0);
}

void ResumeDrawing()
{
	SendMessage(ScrollWnd, WM_SETREDRAW, TRUE, 0);
}

void UpdateCamera()
{
	if (mapImage == nullptr || clientWidth == 0 || clientHeight == 0)
	{
		return;
	}
	updatingCamera = true;
	double num = (double)mapImage->GetWidth() / (double)mapImage->GetHeight();
	double num2 = (double)clientWidth / (double)clientHeight;
	Gdiplus::Point location;
	cameraBounds.GetLocation(&location);
	Gdiplus::Size empty(0, 0);
	if (num2 > num)
	{
		empty.Height = mapImage->GetHeight() / zoom;
		empty.Width = (int)((double)empty.Height * num2);
	}
	else
	{
		empty.Width = mapImage->GetWidth() / zoom;
		empty.Height = (int)((double)empty.Width / num2);
	}
	Gdiplus::Point empty2(0, 0);
	Gdiplus::Size empty3(0, 0);
	if (empty.Width < (INT)mapImage->GetWidth())
	{
		empty2.X = max(0, min((INT)mapImage->GetWidth() - empty.Width, cameraBounds.GetLeft()));
		empty3.Width = (INT)mapImage->GetWidth() * clientWidth / empty.Width;
	}
	else
	{
		empty2.X = ((INT)mapImage->GetWidth() - empty.Width) / 2;
	}
	if (empty.Height < (INT)mapImage->GetHeight())
	{
		empty2.Y = max(0, min((INT)mapImage->GetHeight() - empty.Height, cameraBounds.GetTop()));
		empty3.Height = (INT)mapImage->GetHeight() * clientHeight / empty.Height;
	}
	else
	{
		empty2.Y = ((INT)mapImage->GetHeight() - empty.Height) / 2;
	}
	cameraBounds = Gdiplus::Rect(empty2, empty);
	RecalculateTransforms();
	if (referenceset)
	{
		Gdiplus::Point item = referencePositionsMap;
		Gdiplus::SizeF item2 = referencePositionsClient;
		cameraBounds.GetLocation(&location);
		if (empty3.Width != 0)
		{
			location.X = max(0, min((INT)mapImage->GetWidth() - cameraBounds.Width, (int)((float)item.X - (float)cameraBounds.Width * item2.Width)));
		}
		if (empty3.Height != 0)
		{
			location.Y = max(0, min((INT)mapImage->GetHeight() - cameraBounds.Height, (int)((float)item.Y - (float)cameraBounds.Height * item2.Height)));
		}
		if (empty3.Width || empty3.Height)
		{
			cameraBounds.X = location.X;
			cameraBounds.Y = location.Y;
			RecalculateTransforms();
		}
		referenceset = false;
	}
	SuspendDrawing();
	lastScrollMinSize = AutoScrollMinSize;
	AutoScrollMinSize = empty3;
	UpdateScrollBars();
	Gdiplus::Point loc;
	cameraBounds.GetLocation(&loc);
	AutoScrollPosition = (Gdiplus::Point)MapToClient(Gdiplus::Size(loc.X, loc.Y));
	AutoScrollPosition.X = -AutoScrollPosition.X;
	AutoScrollPosition.Y = -AutoScrollPosition.Y;
	lastScrollPosition = AutoScrollPosition;
	SetScrollPos(ScrollWnd, SB_HORZ, -AutoScrollPosition.X, true);
	SetScrollPos(ScrollWnd, SB_VERT, -AutoScrollPosition.Y, true);
	ResumeDrawing();
	updatingCamera = false;
	InvalidateRect(ScrollWnd, nullptr, FALSE);
}

void SetZoom(int value)
{
	int num = max(minZoom, min(maxZoom, value));
	if (zoom != num)
	{
		zoom = num;
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(ScrollWnd, &pt);
		Gdiplus::Point point;
		point.X = pt.x;
		point.Y = pt.y;
		referencePositionsMap = ClientToMap(point);
		referencePositionsClient = Gdiplus::SizeF((float)point.X / (float)clientWidth, (float)point.Y / (float)clientHeight);
		referenceset = true;
		UpdateCamera();
	}
}

void SetZoomStep(int value)
{
	if (zoomStep != value)
	{
		zoomStep = value;
		SetZoom(zoom / zoomStep * zoomStep);
	}
}

void SetMinZoom(int value)
{
	if (minZoom != value)
	{
		minZoom = value;
		SetZoom(zoom);
	}
}

void SetMaxZoom(int value)
{
	if (maxZoom != value)
	{
		maxZoom = value;
		SetZoom(zoom);
	}
}

void InvalidateScroll()
{
	if (!updatingCamera && (lastScrollPosition.X != AutoScrollPosition.X || lastScrollPosition.Y != AutoScrollPosition.Y))
	{
		Gdiplus::Point pt = lastScrollPosition - AutoScrollPosition;
		Gdiplus::Point pt2 = ClientToMap(Gdiplus::Size(pt.X, pt.Y));
		Gdiplus::Size size = Gdiplus::Size(pt2.X, pt2.Y);
		lastScrollPosition = AutoScrollPosition;
		Gdiplus::Point location;
		cameraBounds.GetLocation(&location);
		if (AutoScrollMinSize.Width != 0)
		{
			location.X = max(0, min((INT)mapImage->GetWidth() - cameraBounds.Width, cameraBounds.GetLeft() + size.Width));
		}
		if (AutoScrollMinSize.Height != 0)
		{
			location.Y = max(0, min((INT)mapImage->GetHeight() - cameraBounds.Height, cameraBounds.GetTop() + size.Height));
		}
		if (AutoScrollMinSize.Width && AutoScrollMinSize.Height)
		{
			cameraBounds.X = location.X;
			cameraBounds.Y = location.Y;
			RecalculateTransforms();
		}
		InvalidateRect(ScrollWnd, nullptr, FALSE);
	}
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
					if (data)
					{
						delete[] data;
					}
					if (graphics)
					{
						delete graphics;
					}
					if (mapImage)
					{
						delete mapImage;
					}
					if (map)
					{
						delete map;
					}
					map = new Map();
					map->Load(fname);
					if (map->isra)
					{
						SetMaxZoom(16);
						SetZoomStep(2);
					}
					else
					{
						SetMaxZoom(8);
						SetZoomStep(1);
					}
					SetMinZoom(1);
					data = new BYTE[map->tilesize * map->width * map->tilesize * map->height * 4];
					mapImage = new Gdiplus::Bitmap(map->tilesize * map->width, map->tilesize * map->height, map->tilesize * map->width * 4, PixelFormat32bppARGB, data);
					graphics = Gdiplus::Graphics::FromImage(mapImage);
					map->Render(graphics);
					UpdateCamera();
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
			WORD cx = LOWORD(lParam);
			WORD cy = HIWORD(lParam);
			SetWindowPos(ScrollWnd, nullptr, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
			InvalidateRect(ScrollWnd, nullptr, TRUE);
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_DESTROY:
		if (data)
		{
			delete[] data;
		}
		if (graphics)
		{
			delete graphics;
		}
		if (mapImage)
		{
			delete mapImage;
		}
		if (map)
		{
			delete map;
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

template <typename T> int sgn(T val)
{
	return (T(0) < val) - (val < T(0));
}

int GetScrollPosition(int bar, UINT code)
{
	SCROLLINFO si = {};
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
	GetScrollInfo(ScrollWnd, bar, &si);
	const int minPos = si.nMin;
	const int maxPos = si.nMax - (si.nPage - 1);
	int result = -1;
	switch (code)
	{
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		result = si.nTrackPos;
		break;
	case SB_LINEUP:
		result = max(si.nPos - 5, minPos);
		break;
	case SB_LINEDOWN:
		result = min(si.nPos + 5, maxPos);
		break;
	case SB_PAGEUP:
		result = max(si.nPos - (int)si.nPage, minPos);
		break;
	case SB_PAGEDOWN:
		result = min(si.nPos + (int)si.nPage, maxPos);
		break;
	case SB_TOP:
		result = minPos;
		break;
	case SB_BOTTOM:
		result = maxPos;
		break;
	}
	return result;
}

LRESULT CALLBACK ScrollWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		{
			mapToViewTransform = new Gdiplus::Matrix();
			viewToPageTransform = new Gdiplus::Matrix();
			compositeTransform = new Gdiplus::Matrix();
			invCompositeTransform = new Gdiplus::Matrix();
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_SIZE:
		{
			WORD cx = LOWORD(lParam);
			WORD cy = HIWORD(lParam);
			clientWidth = cx;
			clientHeight = cy;
			UpdateCamera();
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_HSCROLL:
		{
			int scrollPos = GetScrollPosition(SB_HORZ, LOWORD(wParam));
			if (scrollPos == -1)
			{
				break;
			}
			SetScrollPos(hWnd, SB_HORZ, scrollPos, true);
			AutoScrollPosition.X = -scrollPos;
			InvalidateScroll();
		}
		break;
	case WM_VSCROLL:
		{
			int scrollPos = GetScrollPosition(SB_VERT, LOWORD(wParam));
			if (scrollPos == -1)
			{
				break;
			}
			SetScrollPos(hWnd, SB_VERT, scrollPos, true);
			AutoScrollPosition.Y = -scrollPos;
			InvalidateScroll();
		}
		break;
	case WM_MOUSEWHEEL:
		{
			float delta = GET_WHEEL_DELTA_WPARAM(wParam);
			SetZoom(zoom + zoomStep * sgn(delta));
		}
		break;
	case WM_ERASEBKGND:
		{
			DefWindowProc(hWnd, message, wParam, lParam);
			Gdiplus::Graphics g((HDC)wParam);
			g.Clear(GetSysColor(COLOR_BTNFACE));
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			if (hdc)
			{
				Gdiplus::Graphics g(hdc);
				InvalidateScroll();
				if (mapImage)
				{
					g.SetTransform(compositeTransform);
					g.DrawImage(mapImage, 0, 0);
				}
				fullInvalidation = false;
				EndPaint(hWnd, &ps);
			}
		}
		break;
	case WM_DESTROY:
		{
			delete mapToViewTransform;
			delete viewToPageTransform;
			delete compositeTransform;
			delete invCompositeTransform;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
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
	InitTeamColors();
	InitTextures();
	InitTilesets();
	FreeImage_Initialise();
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = MainWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = nullptr;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = "MainWindow";
	wcex.hIconSm = nullptr;
	RegisterClassEx(&wcex);
	MainWnd = CreateWindowEx(WS_EX_CONTROLPARENT, "MainWindow", "Remaster Map Editor", WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, LoadMenu(hInstance,MAKEINTRESOURCE(IDR_MAINMENU)), hInstance, nullptr);
	WNDCLASSEX wcex2;
	wcex2.cbSize = sizeof(WNDCLASSEX);
	wcex2.style = CS_DBLCLKS;
	wcex2.lpfnWndProc = ScrollWndProc;
	wcex2.cbClsExtra = 0;
	wcex2.cbWndExtra = 0;
	wcex2.hInstance = hInstance;
	wcex2.hIcon = nullptr;
	wcex2.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex2.hbrBackground = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	wcex2.lpszMenuName = nullptr;
	wcex2.lpszClassName = "ScrollWindow";
	wcex2.hIconSm = nullptr;
	RegisterClassEx(&wcex2);
	ShowWindow(MainWnd, SW_MAXIMIZE);
	RECT r;
	GetClientRect(MainWnd, &r);
	clientWidth = r.right - r.left;
	clientHeight = r.bottom - r.top;
	ScrollWnd = CreateWindowEx(0l, "ScrollWindow", nullptr, WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, 0, 0, clientWidth, clientHeight, MainWnd, nullptr, hInstance, nullptr);
	ShowScrollBar(ScrollWnd, SB_HORZ, hscroll);
	ShowScrollBar(ScrollWnd, SB_VERT, vscroll);
	UpdateWindow(MainWnd);
	UpdateWindow(ScrollWnd);
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	FreeImage_DeInitialise();
	ShutdownTilesets();
	ShutdownTextures();
	ShutdownTeamColors();
	CloseMegs();
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return 0;
}
