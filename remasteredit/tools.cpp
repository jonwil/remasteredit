#include "global.h"
#include "tools.h"
#include "map.h"
#include "maprender.h"
#include "mappanel.h"
#include "listview.h"
#include "TemplateType.h"
#include "mainwindow.h"
const char* TemplateGroupNames[]
{
	"Clear",
	"Water",
	"Shore",
	"Slope",
	"Road",
	"Boulder",
	"Brush",
	"Patch",
	"Ford",
	"Falls",
	"River",
	"Bridge",
	"Floor",
	"Wall",
	"Xtra"
};

void NavigationWidget::OnMouseDown(MapPanel* /*panel*/, int Button, Gdiplus::Point Location)
{
	if (Button == VK_MBUTTON)
	{
		lastMouseLocation = Location;
		dragging = true;
	}
}

void NavigationWidget::OnMouseMove(Gdiplus::Point Location)
{
	Gdiplus::Point location = Location;
	if (dragging)
	{
		if (IsKeyDown(VK_MBUTTON))
		{
			dragging = false;
		}
		else
		{
			Gdiplus::Point point = location - lastMouseLocation;
			if (point.X || point.Y)
			{
				mapPanel->AutoScrollPosition = Gdiplus::Point(-mapPanel->AutoScrollPosition.X - point.X, -mapPanel->AutoScrollPosition.Y - point.Y);
			}
		}
	}
	lastMouseLocation = location;
	Gdiplus::Point point2 = mapPanel->ClientToMap(location);
	MouseSubPixel = Gdiplus::Point(point2.X * 24 / cellSize.Width % 24, point2.Y * 24 / cellSize.Height % 24);
	Gdiplus::Point point3(point2.X / cellSize.Width, point2.Y / cellSize.Height);
	if (!(MouseCell.Equals(point3)) && Metrics->Contains(point3))
	{
		Gdiplus::Point mouseCell = MouseCell;
		MouseCell = point3;
		if (tool)
		{
			tool->OnMouseCellChanged(MouseCellChangedEventArgs(mouseCell, MouseCell));
		}
		mapPanel->Invalidate();
	}
}

NavigationWidget::NavigationWidget(MapPanel* MapPanel, CellMetrics* metrics, Gdiplus::Size CellSize) : mapPanel(MapPanel), Metrics(metrics), cellSize(CellSize), mouseoverSize(1, 1), dragging(false), tool(nullptr)
{
	mapPanel->OnMouseMove.insert(this);
	mapPanel->OnMouseDown.insert(this);
}

NavigationWidget::~NavigationWidget()
{
	mapPanel->OnMouseMove.erase(this);
	mapPanel->OnMouseDown.erase(this);
}

void NavigationWidget::Refresh()
{
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(mapPanel->window, &p);
	OnMouseMove(Gdiplus::Point(p.x, p.y));
}

void NavigationWidget::Render(Gdiplus::Graphics* graphics)
{
	Gdiplus::Pen defaultMouseoverPen(Gdiplus::ARGB(Gdiplus::Color::Yellow), 4);
	if (!mouseoverSize.Empty())
	{
		Gdiplus::Rect rect(Gdiplus::Point(MouseCell.X * cellSize.Width, MouseCell.Y * cellSize.Height), cellSize);
		rect.Inflate(cellSize.Width * (mouseoverSize.Width / 2), cellSize.Height * (mouseoverSize.Height / 2));
		graphics->DrawRectangle(&defaultMouseoverPen, rect);
	}
}

void Tool::MapPanelPreRender(RenderEventArgs e)
{
	if (!e.HasCells || e.Cells.size())
	{
		PreRenderMap();
		Map* renderMap = GetRenderMap();
		Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(mapPanel->mapImage);
		auto s = from(e.Cells).where([&](Gdiplus::Point p) {return map->metrics->Contains(p); }).toStdSet();
		Invalidate();
		MapRender::Render(renderMap, g, s, layers);
		delete g;
	}
}

void Tool::MapPanelPostRender(RenderEventArgs e)
{
	PostRenderMap(e.Graphics);
	navigationWidget->Render(e.Graphics);
}

void Tool::Invalidate()
{
	mapPanel->Invalidate(GetRenderMap());
}

void Tool::PreRenderMap()
{
}

Gdiplus::Rect FromLTRB(int left, int top, int right, int bottom)
{
	return Gdiplus::Rect(left, top, right - left, bottom - top);
}

void Tool::PostRenderMap(Gdiplus::Graphics* graphics)
{
	//todo waypoints
	//todo technotriggers
	//todo celltriggers
	if ((layers & MAPLAYER_BOUNDARIES) != 0)
	{
		Gdiplus::Pen pen4(Gdiplus::ARGB(Gdiplus::Color::Cyan), 8);
		Gdiplus::Rect rect = FromLTRB(map->GetBounds().GetLeft() * TileWidth, map->GetBounds().GetTop() * TileHeight, map->GetBounds().GetRight() * TileWidth, map->GetBounds().GetBottom() * TileHeight);
		graphics->DrawRectangle(&pen4, rect);
	}
}

Map* Tool::GetRenderMap()
{
	return map;
}

Tool::Tool(Map* Map, MapPanel* MapPanel, MapLayerFlag Layers) : map(Map), mapPanel(MapPanel), layers(Layers)
{
	mapPanel->OnPreRender = &Tool::MapPanelPreRender;
	mapPanel->OnPostRender = &Tool::MapPanelPostRender;
	navigationWidget = new NavigationWidget(mapPanel, map->metrics, Gdiplus::Size(TileWidth, TileHeight));
}

Tool::~Tool()
{
	delete navigationWidget;
	mapPanel->OnPreRender = nullptr;
	mapPanel->OnPostRender = nullptr;
}

void Tool::SetLayers(MapLayerFlag Layers)
{
	layers = Layers;
	Invalidate();
}

TemplateDialog::TemplateDialog(int x, int y, int width, int height, HWND parent, HINSTANCE instance)
{
	Create(WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT, "TemplateDialog", "Map", WS_CAPTION | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_THICKFRAME, x, y, width, height, parent, nullptr, instance);
	TemplateTypeListView = new ListView(4, 4, 267, 577, window, instance);
	TemplateTypeMapPanel = new MapPanel(instance, 4, 585, 248, 266, window);
	TemplateTypeMapPanel->SetMaxZoom(8);
	TemplateTypeMapPanel->SetMaxZoom(1);
	TemplateTypeMapPanel->SetZoom(1);
	TemplateTypeMapPanel->SetZoomStep(1);
}

TemplateDialog::~TemplateDialog()
{
	delete TemplateTypeListView;
	delete TemplateTypeMapPanel;
	DestroyWindow(window);
}

LRESULT TemplateDialog::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NOTIFY:
		{
			LPNMHDR hdr = reinterpret_cast<LPNMHDR>(lParam);
			if (TemplateTypeListView && hdr->hwndFrom == TemplateTypeListView->GetHandle() && hdr->code == LVN_ITEMCHANGED)
			{
				TemplateTypeListView->OnItemChanged(reinterpret_cast<LPNMLISTVIEW>(lParam));
			}
		}
		break;
	case WM_ERASEBKGND:
		{
			Gdiplus::Graphics g(reinterpret_cast<HDC>(wParam));
			g.Clear(GetSysColor(COLOR_BTNFACE));
		}
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void TemplateTool::SetSelectedTemplateType(const TemplateType* value)
{
	if (SelectedTemplateType == value)
	{
		return;
	}
	if (PlacementMode && SelectedTemplateType != nullptr)
	{
		for (int i = 0; i < SelectedTemplateType->IconHeight; i++)
		{
			for (int j = 0; j < SelectedTemplateType->IconWidth; j++)
			{
				mapPanel->Invalidate(map, Gdiplus::Point(navigationWidget->GetMouseCell().X + j, navigationWidget->GetMouseCell().Y + i));
			}
		}
	}
	SelectedTemplateType = value;
	TemplateTypeListView->BeginUpdate();
	TemplateTypeListView->Widget = nullptr;
	TemplateTypeListView->SelectItemByObject(reinterpret_cast<LPARAM>(SelectedTemplateType));
	if (TemplateTypeListView->GetSelectedIndex() != -1)
	{
		TemplateTypeListView->EnsureVisible(TemplateTypeListView->GetSelectedIndex());
	}
	TemplateTypeListView->Widget = this;
	TemplateTypeListView->EndUpdate();
	if (PlacementMode && SelectedTemplateType != nullptr)
	{
		for (int i = 0; i < SelectedTemplateType->IconHeight; i++)
		{
			for (int j = 0; j < SelectedTemplateType->IconWidth; j++)
			{
				mapPanel->Invalidate(map, Gdiplus::Point(navigationWidget->GetMouseCell().X + j, navigationWidget->GetMouseCell().Y + i));
			}
		}
	}
	RefreshMapPanel();
}

void TemplateTool::SetSelectedIcon(Gdiplus::Point value)
{
	if (SelectedIcon.X == value.X && SelectedIcon.Y == value.Y && value.X != -1 && value.Y != -1)
	{
		return;
	}
	SelectedIcon = value;
	TemplateTypeMapPanel->zoomed = true;
	TemplateTypeMapPanel->Invalidate();
	if (!PlacementMode || SelectedTemplateType == nullptr)
	{
		return;
	}
	for (int i = 0; i < SelectedTemplateType->IconHeight; i++)
	{
		for (int j = 0; j < SelectedTemplateType->IconWidth; j++)
		{
			mapPanel->Invalidate(map, Gdiplus::Point(navigationWidget->GetMouseCell().X + j, navigationWidget->GetMouseCell().Y + i));
		}
	}
}

void TemplateTool::OnMouseDown(MapPanel* panel, int Button, Gdiplus::Point /*Location*/)
{
	if (panel == TemplateTypeMapPanel)
	{
		if (SelectedTemplateType == nullptr || SelectedTemplateType->IconWidth * SelectedTemplateType->IconHeight == 1)
		{
			SetSelectedIcon(Gdiplus::Point(-1,-1));
		}
		else if (Button == VK_LBUTTON)
		{
			Gdiplus::Point mouseCell = TemplateTypeNavigationWidget->GetMouseCell();
			if (mouseCell.X >= 0 && mouseCell.X < SelectedTemplateType->IconWidth && mouseCell.Y >= 0 && mouseCell.Y < SelectedTemplateType->IconHeight && SelectedTemplateType->IconMask[mouseCell.X + SelectedTemplateType->IconWidth * mouseCell.Y])
			{
				SetSelectedIcon(mouseCell);
			}
		}
		else if (Button == VK_RBUTTON)
		{
			SetSelectedIcon(Gdiplus::Point(-1, -1));
		}
	}
	else if (panel == mapPanel)
	{
		if (BoundsMode)
		{
			Dragging = true;
			StartDrag = navigationWidget->GetMouseCell();
			map->metrics->Clip(StartDrag, Gdiplus::Size(1, 1));
			UpdateTooltip();
		}
		else if (PlacementMode)
		{
			if (Button == VK_LBUTTON)
			{
				SetTemplate(navigationWidget->GetMouseCell());
			}
			else if (Button == VK_RBUTTON)
			{
				RemoveTemplate(navigationWidget->GetMouseCell());
			}
		}
		else if (Button == VK_LBUTTON || Button == VK_RBUTTON)
		{
			PickTemplate(navigationWidget->GetMouseCell(), Button == VK_LBUTTON);
		}
	}
}

void TemplateTool::TemplateMapPanelPostRender(RenderEventArgs e)
{
	if (SelectedIcon.X != -1 && SelectedIcon.Y != -1)
	{
		Gdiplus::Pen pen(Gdiplus::ARGB(Gdiplus::Color::Yellow), 2);
		Gdiplus::Size size(32, 32);
		Gdiplus::Rect rect(Gdiplus::Point(SelectedIcon.X * size.Width, SelectedIcon.Y * size.Height), size);
		e.Graphics->DrawRectangle(&pen, rect);
	}
	if (SelectedTemplateType != nullptr)
	{
		Gdiplus::StringFormat stringFormat;
		stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		Gdiplus::SolidBrush brush = Gdiplus::SolidBrush(FromArgb(128, Gdiplus::ARGB(Gdiplus::Color::Black)));
		Gdiplus::SolidBrush brush2 = Gdiplus::SolidBrush(Gdiplus::Color(Gdiplus::ARGB(Gdiplus::Color::White)));
		wchar_t text[100];
		_swprintf(text, L"%S (%dx%d)", SelectedTemplateType->Name.c_str(), SelectedTemplateType->IconWidth, SelectedTemplateType->IconHeight);
		NONCLIENTMETRICS cmetrics;
		cmetrics.cbSize = sizeof(cmetrics);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(cmetrics), &cmetrics, 0);
		HDC dc = GetDC(nullptr);
		Gdiplus::Font font(dc, &cmetrics.lfCaptionFont);
		Gdiplus::SizeF size2;
		e.Graphics->MeasureString(text, wcslen(text), &font, Gdiplus::SizeF(0, 0), nullptr, &size2);
		size2 = size2 + Gdiplus::SizeF(6, 6);
		Gdiplus::RectF rectangleF(Gdiplus::PointF(0, 0), size2);
		Gdiplus::Matrix* m = new Gdiplus::Matrix;
		e.Graphics->SetTransform(m);
		e.Graphics->FillRectangle(&brush, rectangleF);
		e.Graphics->DrawString(text, wcslen(text), &font, rectangleF, &stringFormat, &brush2);
	}
}

void TemplateTool::OnKeyDown(int KeyCode)
{
	if (KeyCode == VK_SHIFT)
	{
		EnterPlacementMode();
	}
	else if (KeyCode == VK_CONTROL)
	{
		EnterBoundsMode();
	}
}

void TemplateTool::OnKeyUp(int KeyCode)
{
	if (KeyCode == VK_SHIFT || KeyCode == VK_CONTROL)
	{
		ExitAllModes();
	}
}

void TemplateTool::OnMouseMove(Gdiplus::Point /*Location*/)
{
	if (!PlacementMode && IsKeyDown(VK_SHIFT))
	{
		EnterPlacementMode();
	}
	else if (!BoundsMode && IsKeyDown(VK_CONTROL))
	{
		EnterBoundsMode();
	}
	else if (!IsKeyDown(VK_SHIFT) && !IsKeyDown(VK_CONTROL) && !IsKeyDown(VK_MENU))
	{
		ExitAllModes();
	}
}

void TemplateTool::OnMouseUp(int /*Button*/)
{
	Gdiplus::Rect bounds;
	Gdiplus::Rect oldBounds;
	if (BoundsMode)
	{
		if (Dragging)
		{
			Gdiplus::Point location = navigationWidget->GetMouseCell();
			map->metrics->Clip(location, Gdiplus::Size(1, 1));
			bounds = Gdiplus::Rect(StartDrag, Gdiplus::Size(location.X, location.Y) - Gdiplus::Size(StartDrag.X, StartDrag.Y) + Gdiplus::Size(1, 1));
			if (bounds.Width > 0 && bounds.Height > 0 && !map->GetBounds().Equals(bounds))
			{
				map->SetBounds(bounds);
			}
			Dragging = false;
			mapPanel->Invalidate();
			UpdateStatus();
			UpdateTooltip();
		}
	}
}

void TemplateTool::OnMouseCellChanged(MouseCellChangedEventArgs e)
{
	if (Dragging)
	{
		mapPanel->Invalidate();
	}
	else if (PlacementMode)
	{
		if (IsKeyDown(VK_RBUTTON))
		{
			RemoveTemplate(navigationWidget->GetMouseCell());
		}
		if (SelectedTemplateType == nullptr)
		{
			return;
		}
		Gdiplus::Point array[2] = { e.OldCell, e.NewCell };
		for (int i = 0; i < _countof(array); i++)
		{
			for (int j = 0; j < SelectedTemplateType->IconHeight; j++)
			{
				Gdiplus::Point point = array[i];
				for (int k = 0; k < SelectedTemplateType->IconWidth; k++)
				{
					mapPanel->Invalidate(map, Gdiplus::Point(point.X + k, point.Y + j));
				}
			}
		}
	}
	else if (IsKeyDown(VK_LBUTTON) || IsKeyDown(VK_RBUTTON))
	{
		PickTemplate(navigationWidget->GetMouseCell(), IsKeyDown(VK_LBUTTON));
	}
}

void TemplateTool::RefreshMapPanel()
{
	if (TemplateTypeNavigationWidget != nullptr)
	{
		delete TemplateTypeNavigationWidget->GetMetrics();
		delete TemplateTypeNavigationWidget;
		TemplateTypeNavigationWidget = nullptr;
	}
	if (SelectedTemplateType != nullptr)
	{
		TemplateTypeMapPanel->mapImage = SelectedTemplateType->Thumbnail;
		CellMetrics *metrics = new CellMetrics(SelectedTemplateType->IconWidth, SelectedTemplateType->IconHeight);
		TemplateTypeNavigationWidget = new NavigationWidget(TemplateTypeMapPanel, metrics, Gdiplus::Size(32, 32));
		TemplateTypeNavigationWidget->SetMouseoverSize(Gdiplus::Size(0,0));
	}
	else
	{
		TemplateTypeMapPanel->mapImage = nullptr;
	}
}

void TemplateTool::SetTemplate(Gdiplus::Point location)
{
	if (SelectedTemplateType == nullptr)
	{
		return;
	}
	if (SelectedIcon.X != -1 && SelectedIcon.Y != -1)
	{
		int cell;
		if (map->metrics->GetCell(location, cell))
		{
			int icon = SelectedIcon.Y * SelectedTemplateType->IconWidth + SelectedIcon.X;
			Template* oldTemplate = map->templates->Get(cell);
			if (oldTemplate->type != SelectedTemplateType || oldTemplate->icon != icon)
			{
				Template* newTemplate = new Template;
				newTemplate->type = SelectedTemplateType;
				newTemplate->icon = icon;
				map->templates->Set(cell, newTemplate);
				mapPanel->Invalidate(map, cell);
				if (oldTemplate)
				{
					delete oldTemplate;
				}
			}
		}
		return;
	}
	int num3 = 0;
	for (int i = 0; i < SelectedTemplateType->IconHeight; i++)
	{
		for (int j = 0; j < SelectedTemplateType->IconWidth; j++)
		{
			if (SelectedTemplateType->IconMask[i * SelectedTemplateType->IconWidth + j])
			{
				Gdiplus::Point location3(location.X + j, location.Y + i);
				int cell3;
				if (map->metrics->GetCell(location3, cell3))
				{
					Template* oldTemplate = map->templates->Get(cell3);
					if (oldTemplate->type != SelectedTemplateType || oldTemplate->icon != num3)
					{
						Template* newTemplate = new Template;
						newTemplate->type = SelectedTemplateType;
						newTemplate->icon = num3;
						map->templates->Set(cell3, newTemplate);
						mapPanel->Invalidate(map, cell3);
						if (oldTemplate)
						{
							delete oldTemplate;
						}
					}
				}
			}
			num3++;
		}
	}
}

void TemplateTool::RemoveTemplate(Gdiplus::Point location)
{
	if (SelectedTemplateType == nullptr)
	{
		return;
	}
	if (SelectedIcon.X != -1 && SelectedIcon.Y != -1)
	{
		int cell;
		if (map->metrics->GetCell(location, cell))
		{
			Template* oldTemplate = map->templates->Get(cell);
			map->templates->Set(cell, nullptr);
			mapPanel->Invalidate(map, cell);
			if (oldTemplate)
			{
				delete oldTemplate;
			}
		}
		return;
	}
	int num3 = 0;
	for (int i = 0; i < SelectedTemplateType->IconHeight; i++)
	{
		for (int j = 0; j < SelectedTemplateType->IconWidth; j++)
		{
			if (SelectedTemplateType->IconMask[i * SelectedTemplateType->IconWidth + j])
			{
				Gdiplus::Point location3(location.X + j, location.Y + i);
				int cell3;
				if (map->metrics->GetCell(location3, cell3))
				{
					Template* oldTemplate = map->templates->Get(cell3);
					map->templates->Set(cell3, nullptr);
					mapPanel->Invalidate(map, cell3);
					if (oldTemplate)
					{
						delete oldTemplate;
					}
				}
			}
			num3++;
		}
	}
}

void TemplateTool::EnterPlacementMode()
{
	if (PlacementMode || BoundsMode)
	{
		return;
	}
	PlacementMode = true;
	navigationWidget->SetMouseoverSize(Gdiplus::Size(0,0));
	if (SelectedTemplateType != nullptr)
	{
		for (int i = 0; i < SelectedTemplateType->IconHeight; i++)
		{
			for (int j = 0; j < SelectedTemplateType->IconWidth; j++)
			{
				mapPanel->Invalidate(map, Gdiplus::Point(navigationWidget->GetMouseCell().X + j, navigationWidget->GetMouseCell().Y + i));
			}
		}
	}
	UpdateStatus();
}

void TemplateTool::EnterBoundsMode()
{
	if (BoundsMode || PlacementMode)
	{
		return;
	}
	BoundsMode = true;
	navigationWidget->SetMouseoverSize(Gdiplus::Size(0, 0));
	if (SelectedTemplateType != nullptr)
	{
		for (int i = 0; i < SelectedTemplateType->IconHeight; i++)
		{
			for (int j = 0; j < SelectedTemplateType->IconWidth; j++)
			{
				mapPanel->Invalidate(map, Gdiplus::Point(navigationWidget->GetMouseCell().X + j, navigationWidget->GetMouseCell().Y + i));
			}
		}
	}
	UpdateTooltip();
	UpdateStatus();
}

void TemplateTool::ExitAllModes()
{
	if (!PlacementMode && !BoundsMode)
	{
		return;
	}
	BoundsMode = false;
	Dragging = false;
	StartDrag = Gdiplus::Point(0,0);
	PlacementMode = false;
	navigationWidget->SetMouseoverSize(Gdiplus::Size(1, 1));
	if (SelectedTemplateType != nullptr)
	{
		for (int i = 0; i < SelectedTemplateType->IconHeight; i++)
		{
			for (int j = 0; j < SelectedTemplateType->IconWidth; j++)
			{
				mapPanel->Invalidate(map, Gdiplus::Point(navigationWidget->GetMouseCell().X + j, navigationWidget->GetMouseCell().Y + i));
			}
		}
	}
	UpdateTooltip();
	UpdateStatus();
}

void TemplateTool::UpdateTooltip()
{
	//todo update tooltip
}

void TemplateTool::PickTemplate(Gdiplus::Point location, bool wholeTemplate)
{
	int cell;
	if (map->metrics->GetCell(location, cell))
	{
		Template *t = map->templates->Get(cell);
		if (t != nullptr)
		{
			SetSelectedTemplateType(t->type);
		}
		else
		{
			if (map->isra)
			{
				SetSelectedTemplateType(TemplateType::PointersRA[0]);
			}
			else
			{
				SetSelectedTemplateType(TemplateType::PointersTD[0]);
			}
		}
		if (!wholeTemplate && SelectedTemplateType->IconWidth * SelectedTemplateType->IconHeight > 1)
		{
			int num = 0;
			if (t)
			{
				num = t->icon;
			}
			SetSelectedIcon(Gdiplus::Point(num % SelectedTemplateType->IconWidth, num / SelectedTemplateType->IconWidth));
		}
		else
		{
			SetSelectedIcon(Gdiplus::Point(-1,-1));
		}
	}
}

void TemplateTool::UpdateStatus()
{
	if (PlacementMode)
	{
		MainWindow::mainwindow->statusbar->SetText(0, "Left-Click to place template, Right-Click to clear template");
	}
	else if (BoundsMode)
	{
		if (Dragging)
		{
			MainWindow::mainwindow->statusbar->SetText(0, "Release left button to end dragging map bounds");
		}
		else
		{
			MainWindow::mainwindow->statusbar->SetText(0, "Left-Click to start dragging map bounds");
		}
	}
	else
	{
		MainWindow::mainwindow->statusbar->SetText(0, "Shift to enter placement mode, Ctrl to enter map bounds mode, Left-Click to pick whole template, Right-Click to pick individual template tile");
	}
}

void TemplateTool::PreRenderMap()
{
	Tool::PreRenderMap();
	if (PreviewMap != map)
	{
		delete PreviewMap;
	}
	PreviewMap = map->Clone();
	if (!PlacementMode)
	{
		return;
	}
	Gdiplus::Point mouseCell = navigationWidget->GetMouseCell();
	if (SelectedTemplateType == nullptr)
	{
		return;
	}
	if (SelectedIcon.X != -1 && SelectedIcon.Y != -1)
	{
		int cell;
		if (PreviewMap->metrics->GetCell(mouseCell, cell))
		{
			int icon = SelectedIcon.Y * SelectedTemplateType->IconWidth + SelectedIcon.X;
			Template* oldTemplate = PreviewMap->templates->Get(cell);
			if (oldTemplate->type != SelectedTemplateType || oldTemplate->icon != icon)
			{
				Template* newTemplate = new Template;
				newTemplate->type = SelectedTemplateType;
				newTemplate->icon = icon;
				PreviewMap->templates->Set(cell, newTemplate);
				if (oldTemplate)
				{
					delete oldTemplate;
				}
			}
		}
		return;
	}
	int num3 = 0;
	for (int i = 0; i < SelectedTemplateType->IconHeight; i++)
	{
		for (int j = 0; j < SelectedTemplateType->IconWidth; j++)
		{
			if (SelectedTemplateType->IconMask[i * SelectedTemplateType->IconWidth + j])
			{
				Gdiplus::Point location3(mouseCell.X + j, mouseCell.Y + i);
				int cell3;
				if (PreviewMap->metrics->GetCell(location3, cell3))
				{
					Template* oldTemplate = PreviewMap->templates->Get(cell3);
					if (oldTemplate->type != SelectedTemplateType || oldTemplate->icon != num3)
					{
						Template* newTemplate = new Template;
						newTemplate->type = SelectedTemplateType;
						newTemplate->icon = num3;
						PreviewMap->templates->Set(cell3, newTemplate);
						if (oldTemplate)
						{
							delete oldTemplate;
						}
					}
				}
			}
			num3++;
		}
	}
}

void TemplateTool::PostRenderMap(Gdiplus::Graphics* graphics)
{
	Tool::PostRenderMap(graphics);
	if (BoundsMode)
	{
		if (Dragging)
		{
			Gdiplus::Point location = navigationWidget->GetMouseCell();
			map->metrics->Clip(location, Gdiplus::Size(1, 1));
			Gdiplus::Rect rectangle = Gdiplus::Rect(StartDrag, Gdiplus::Size(location.X, location.Y) - Gdiplus::Size(StartDrag.X, StartDrag.Y) + Gdiplus::Size(1, 1));
			if (rectangle.Width > 0 && rectangle.Height > 0)
			{
				Gdiplus::Rect rect = FromLTRB(rectangle.GetLeft() * TileWidth, rectangle.GetTop() * TileHeight, rectangle.GetRight() * TileWidth, rectangle.GetBottom() * TileHeight);
				Gdiplus::Pen pen4(Gdiplus::ARGB(Gdiplus::Color::Green), 8);
				graphics->DrawRectangle(&pen4, rect);
			}
		}
	}
	else if (PlacementMode)
	{
		Gdiplus::Point mouseCell = navigationWidget->GetMouseCell();
		if (SelectedTemplateType != nullptr)
		{
			Gdiplus::Rect rect2(mouseCell.X * TileWidth, mouseCell.Y * TileHeight, ((SelectedIcon.X != -1 && SelectedIcon.Y != -1) ? 1 : SelectedTemplateType->IconWidth) * TileWidth, ((SelectedIcon.X != -1 && SelectedIcon.Y != -1) ? 1 : SelectedTemplateType->IconHeight) * TileHeight);
			Gdiplus::Pen pen4(Gdiplus::ARGB(Gdiplus::Color::Green), 4);
			graphics->DrawRectangle(&pen4, rect2);
		}
	}
}

void TemplateTool::OnSelChanged()
{
	SetSelectedTemplateType(reinterpret_cast<TemplateType *>(TemplateTypeListView->GetSelectedObject()));
	SetSelectedIcon(Gdiplus::Point(-1,-1));
}

TemplateTool::TemplateTool(Map* map, MapPanel* mapPanel, MapLayerFlag layers, ListView* templateTypeListView, MapPanel* templateTypeMapPanel) : Tool(map, mapPanel, layers), TemplateTypeMapPanel(templateTypeMapPanel), TemplateTypeListView(templateTypeListView), PreviewMap(map), PlacementMode(false), BoundsMode(false), Dragging(false), StartDrag(0, 0), SelectedIcon(-1, -1), TemplateTypeNavigationWidget(nullptr)
{
	mapPanel->OnMouseDown.insert(this);
	mapPanel->OnMouseUp.insert(this);
	mapPanel->OnMouseMove.insert(this);
	mapPanel->OnKeyDown.insert(this);
	mapPanel->OnKeyUp.insert(this);
	templateTypeListView->Widget = this;
	auto f = from(TemplateType::PointersTD);
	if (map->isra)
	{
		f = from(TemplateType::PointersRA);
	}
	auto th = f.where([map](TemplateType* t) {return (t->Theater & map->theaterid) != 0 && t->Thumbnail != 0 && (t->Flag & TEMPLATETYPE_CLEAR) == 0; });
	int width = th.Max([](TemplateType* t) {return t->Thumbnail->GetWidth(); })->Thumbnail->GetWidth();
	int height = th.Max([](TemplateType* t) {return t->Thumbnail->GetHeight(); })->Thumbnail->GetHeight();
	List = new ImageList(Gdiplus::Size(width, height));
	templateTypeListView->BeginUpdate();
	std::vector<int> indexes;
	for (int i = 0; i < GROUP_COUNT; i++)
	{
		auto v = th.where([i](TemplateType* t) {return t->Group == i; }).toStdVector();
		if (v.size())
		{
			for (auto t : v)
			{
				indexes.push_back(List->AddImage(t->Thumbnail));
			}
		}
	}
	templateTypeListView->SetImageList(List);
	int index = 0;
	for (int i = 0; i < GROUP_COUNT; i++)
	{
		auto v = th.where([i](TemplateType* t) {return t->Group == i; }).toStdVector();
		if (v.size())
		{
			int id = templateTypeListView->AddGroup(TemplateGroupNames[i]);
			for (auto t : v)
			{
				templateTypeListView->AddItem(t->Name.c_str(), id, indexes[index++], reinterpret_cast<LPARAM>(t));
			}
		}
	}
	templateTypeListView->EndUpdate();
	templateTypeMapPanel->OnMouseDown.insert(this);
	templateTypeMapPanel->currentTemplateTool = this;
	templateTypeMapPanel->OnTemplatePostRender = &TemplateTool::TemplateMapPanelPostRender;
	templateTypeMapPanel->SetMaxZoom(1);
	navigationWidget->tool = this;
	SetSelectedTemplateType(th.toStdVector()[0]);
	UpdateStatus();
}

TemplateTool::~TemplateTool()
{
	mapPanel->OnMouseDown.erase(this);
	mapPanel->OnMouseUp.erase(this);
	mapPanel->OnMouseMove.erase(this);
	mapPanel->OnKeyDown.erase(this);
	mapPanel->OnKeyUp.erase(this);
	TemplateTypeListView->Widget = nullptr;
	navigationWidget->tool = nullptr;
	TemplateTypeMapPanel->currentTemplateTool = nullptr;
	TemplateTypeMapPanel->OnTemplatePostRender = nullptr;
	delete List;
	if (PreviewMap != map)
	{
		delete PreviewMap;
	}
}
