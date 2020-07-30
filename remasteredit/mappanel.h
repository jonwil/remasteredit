#pragma once
#include "window.h"
class Map;
class Overlapper;
class Tool;
class TemplateTool;
class MapPanel;
class Widget
{
public:
	virtual void OnMouseDown(MapPanel* /*panel*/, int /*Button*/, Gdiplus::Point /*Location*/) {}
	virtual void OnMouseDoubleClick() {}
	virtual void OnMouseMove(Gdiplus::Point /*Location*/) {}
	virtual void OnMouseUp(int /*Button*/) {}
	virtual void OnKeyDown(int /*KeyCode*/) {}
	virtual void OnKeyUp(int /*KeyCode*/) {}
	virtual void OnSelChanged() {}
};
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
	void (Tool::* OnPreRender)(RenderEventArgs e);
	void (Tool::* OnPostRender)(RenderEventArgs e);
	void (TemplateTool::* OnTemplatePostRender)(RenderEventArgs e);
	std::set<Widget*> OnMouseDown;
	std::set<Widget*> OnMouseDoubleClick;
	std::set<Widget*> OnMouseMove;
	std::set<Widget*> OnMouseUp;
	std::set<Widget*> OnKeyDown;
	std::set<Widget*> OnKeyUp;
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
	bool zoomed;
	int clientWidth;
	int clientHeight;
	Gdiplus::Size AutoScrollMinSize;
	Gdiplus::Size lastScrollMinSize;
	Gdiplus::Point AutoScrollPosition;
	bool hscroll;
	bool vscroll;
	Map* map;
	BYTE* data;
	Tool* currentTool;
	TemplateTool* currentTemplateTool;
	bool focusOnMouseEnter;
	MapPanel(HINSTANCE hInstance, DWORD x, DWORD y, DWORD width, DWORD height, HWND parent) : updatingCamera(false), lastScrollPosition(0, 0), referencePositionsSet(false), mapToViewTransform(nullptr), viewToPageTransform(nullptr), compositeTransform(nullptr), invCompositeTransform(nullptr), fullInvalidation(false), mapImage(nullptr), minZoom(1), maxZoom(8), zoomStep(1), zoom(1), clientWidth(width), clientHeight(height), AutoScrollPosition(0, 0), hscroll(false), vscroll(false), map(nullptr), data(nullptr), OnPreRender(nullptr), OnPostRender(nullptr), zoomed(false), focusOnMouseEnter(false)
	{
		Create(0l, "ScrollWindow", nullptr, WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, x, y, width, height, parent, nullptr, hInstance);
		ShowScrollBar(window, SB_HORZ, hscroll);
		ShowScrollBar(window, SB_VERT, vscroll);
	}
	~MapPanel()
	{
	}
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	Gdiplus::Point MapToClient(Gdiplus::Point point);
	Gdiplus::Size MapToClient(Gdiplus::Size size);
	Gdiplus::Rect MapToClient(Gdiplus::Rect rectangle);
	Gdiplus::Point ClientToMap(Gdiplus::Point point);
	Gdiplus::Point ClientToMap(Gdiplus::Size size);
	Gdiplus::Rect ClientToMap(Gdiplus::Rect rectangle);
	void MouseMove(Gdiplus::Point p);
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
};

extern MapPanel* MainPanel;