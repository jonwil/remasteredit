#pragma once
#include "window.h"
class Map;
class Overlapper;
class RenderEventArgs
{
public:
	Gdiplus::Graphics *Graphics;
	bool HasCells;
	std::set<Gdiplus::Point> Cells;
	RenderEventArgs(Gdiplus::Graphics* graphics, std::set<Gdiplus::Point> cells, bool hascells)
	{
		Graphics = graphics;
		Cells = cells;
		HasCells = hascells;
	}
};
class MapPanel : public Window
{
public:
	bool updatingCamera;
	Gdiplus::Rect cameraBounds;
	Gdiplus::Point lastScrollPosition;
	bool referencePositionsSet;
	Gdiplus::Point referencePositionsMap;
	Gdiplus::SizeF referencePositionsClient;
	Gdiplus::Matrix* mapToViewTransform;
	Gdiplus::Matrix* viewToPageTransform;
	Gdiplus::Matrix* compositeTransform;
	Gdiplus::Matrix* invCompositeTransform;
	std::set<Gdiplus::Point> invalidateCells;
	bool fullInvalidation;
	Gdiplus::Bitmap* mapImage;
	int minZoom;
	int maxZoom;
	int zoomStep;
	int zoom;
	int clientWidth;
	int clientHeight;
	Gdiplus::Size AutoScrollMinSize;
	Gdiplus::Size lastScrollMinSize;
	Gdiplus::Point AutoScrollPosition;
	bool hscroll;
	bool vscroll;
	Map* map;
	BYTE* data;
	MapPanel(HINSTANCE hInstance, DWORD width, DWORD height, HWND parent) : updatingCamera(false), lastScrollPosition(0, 0), referencePositionsSet(false), mapToViewTransform(nullptr), viewToPageTransform(nullptr), compositeTransform(nullptr), invCompositeTransform(nullptr), fullInvalidation(false), mapImage(nullptr), minZoom(1), maxZoom(8), zoomStep(1), zoom(1), clientWidth(width), clientHeight(height), AutoScrollPosition(0, 0), hscroll(false), vscroll(false), map(nullptr), data(nullptr)
	{
		panel = this;
		Create(0l, "ScrollWindow", nullptr, WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, 0, 0, width, height, parent, nullptr, hInstance);
		ShowScrollBar(window, SB_HORZ, hscroll);
		ShowScrollBar(window, SB_VERT, vscroll);
	}
	~MapPanel()
	{
		panel = nullptr;
	}
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	Gdiplus::Point MapToClient(Gdiplus::Point point);
	Gdiplus::Size MapToClient(Gdiplus::Size size);
	Gdiplus::Rect MapToClient(Gdiplus::Rect rectangle);
	Gdiplus::Point ClientToMap(Gdiplus::Point point);
	Gdiplus::Point ClientToMap(Gdiplus::Size size);
	Gdiplus::Rect ClientToMap(Gdiplus::Rect rectangle);
	void UpdateCamera();
	void RecalculateTransforms();
	void InvalidateScroll();
	void UpdateScrollBars();
	void SuspendDrawing();
	void ResumeDrawing();
	void SetZoom(int value);
	void SetZoomStep(int value);
	void SetMinZoom(int value);
	void SetMaxZoom(int value);
	int GetScrollPosition(int bar, UINT code);
	static MapPanel* panel;
	void FileOpen(const char* fname);
	void Invalidate();
	void Invalidate(Map* invalidateMap);
	void Invalidate(Map* invalidateMap, Gdiplus::Rect cellBounds);
	void Invalidate(Map* invalidateMap, std::vector<Gdiplus::Rect> cellBounds);
	void Invalidate(Map* invalidateMap, Gdiplus::Point location);
	void Invalidate(Map* invalidateMap, std::vector<Gdiplus::Point> locations);
	void Invalidate(Map* invalidateMap, int cell);
	void Invalidate(Map* invalidateMap, std::vector<int> cells);
	void Invalidate(Map* invalidateMap, Overlapper *overlapper);
	void PreRender(RenderEventArgs e);
	void PostRender(RenderEventArgs e);
};