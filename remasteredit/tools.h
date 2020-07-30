#pragma once
#include "mappanel.h"
#include "window.h"
class CellMetrics;
class TemplateType;
class ListView;
class ImageList;
class Template;
class MouseCellChangedEventArgs
{
public:
	Gdiplus::Point OldCell;
	Gdiplus::Point NewCell;
	MouseCellChangedEventArgs(Gdiplus::Point oldCell, Gdiplus::Point newCell)
	{
		OldCell = oldCell;
		NewCell = newCell;
	}
};
class NavigationWidget : public Widget
{
private:
	MapPanel* mapPanel;
	Gdiplus::Size cellSize;
	bool dragging;
	Gdiplus::Point lastMouseLocation;
	Gdiplus::Size mouseoverSize;
	CellMetrics* Metrics;
	Gdiplus::Point MouseCell;
	Gdiplus::Point MouseSubPixel;
	void OnMouseDown(MapPanel *panel, int Button, Gdiplus::Point Location) override;
	void OnMouseMove(Gdiplus::Point Location) override;
public:
	Tool* tool;
	Gdiplus::Point GetMouseCell() { return MouseCell; }
	Gdiplus::Point GetMouseSubPixel() { return MouseSubPixel; }
	Gdiplus::Size GetMouseoverSize() { return mouseoverSize; }
	void SetMouseoverSize(Gdiplus::Size value) { mouseoverSize = ((!value.Empty()) ? Gdiplus::Size(value.Width | 1, value.Height | 1) : Gdiplus::Size(0, 0)); }
	NavigationWidget(MapPanel *mapPanel, CellMetrics *metrics, Gdiplus::Size cellSize);
	~NavigationWidget();
	void Refresh();
	void Render(Gdiplus::Graphics* graphics);
	CellMetrics* GetMetrics() { return Metrics; }
};
class Tool : public Widget
{
private:
	MapLayerFlag layers;
	void MapPanelPostRender(RenderEventArgs e);
	void MapPanelPreRender(RenderEventArgs e);
protected:
	Map* map;
	MapPanel* mapPanel;
	NavigationWidget* navigationWidget;
	void Invalidate();
	virtual void PreRenderMap();
	virtual void PostRenderMap(Gdiplus::Graphics* graphics);
	virtual Map* GetRenderMap();
public:
	virtual void OnMouseCellChanged(MouseCellChangedEventArgs /*e*/) {};
	Tool(Map* Map, MapPanel* MapPanel, MapLayerFlag Layers);
	virtual ~Tool();
	void SetLayers(MapLayerFlag Layers);
};

class TemplateDialog : public Window
{
public:
	ListView* TemplateTypeListView;
	MapPanel* TemplateTypeMapPanel;
	TemplateDialog(int x, int y, int width, int height, HWND parent, HINSTANCE instance);
	~TemplateDialog();
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
};

class TemplateTool : public Tool
{
private:
	MapPanel* TemplateTypeMapPanel;
	ListView* TemplateTypeListView;
	ImageList* List;
	Map* PreviewMap;
	bool PlacementMode;
	bool BoundsMode;
	bool Dragging;
	Gdiplus::Point StartDrag;
	const TemplateType *SelectedTemplateType;
	Gdiplus::Point SelectedIcon;
	NavigationWidget* TemplateTypeNavigationWidget;
	Map* GetRenderMap() override { return PreviewMap; }
	void SetSelectedTemplateType(const TemplateType* value);
	void SetSelectedIcon(Gdiplus::Point value);
	void OnMouseDown(MapPanel* panel, int Button, Gdiplus::Point Location) override;
	void TemplateMapPanelPostRender(RenderEventArgs e);
	void OnKeyDown(int KeyCode) override;
	void OnKeyUp(int KeyCode) override;
	void OnMouseMove(Gdiplus::Point Location) override;
	void OnMouseUp(int Button) override;
	void OnMouseCellChanged(MouseCellChangedEventArgs e) override;
	void RefreshMapPanel();
	void SetTemplate(Gdiplus::Point location);
	void RemoveTemplate(Gdiplus::Point location);
	void EnterPlacementMode();
	void EnterBoundsMode();
	void ExitAllModes();
	void UpdateTooltip();
	void PickTemplate(Gdiplus::Point location, bool wholeTemplate);
	void UpdateStatus();
	void PreRenderMap() override;
	void PostRenderMap(Gdiplus::Graphics *graphics) override;
	void OnSelChanged() override;
public:
	TemplateTool(Map* map, MapPanel* mapPanel, MapLayerFlag layers, ListView* templateTypeListView, MapPanel* templateTypeMapPanel);
	~TemplateTool();
};
