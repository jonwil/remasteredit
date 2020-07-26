#include "global.h"
#include "mappanel.h"
#include "map.h"
#include "maprender.h"
#include "mainwindow.h"
#include "TemplateType.h"
#include "smudgetype.h"
#include "overlaytype.h"
#include "terraintype.h"
#include "infantrytype.h"
#include "structtype.h"
#include "unittype.h"
#include "vesseltype.h"
#include "AircraftType.h"
#include "textman.h"
#include <windowsx.h>
#include <sstream>
template <typename T> int sgn(T val)
{
	return (T(0) < val) - (val < T(0));
}

MapPanel* MapPanel::panel = nullptr;

LRESULT MapPanel::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_MOUSEMOVE:
		{
			Gdiplus::Point p(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			OnMouseMove(p);
		}
		break;
	case WM_ERASEBKGND:
		{
			DefWindowProc(hWnd, message, wParam, lParam);
			Gdiplus::Graphics g(reinterpret_cast<HDC>(wParam));
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
				std::set<Gdiplus::Point> cells;
				if (!fullInvalidation)
				{
					cells = invalidateCells;
				}
				PreRender(RenderEventArgs(&g, cells, !fullInvalidation));
				if (mapImage)
				{
					g.SetTransform(compositeTransform);
					g.DrawImage(mapImage, 0, 0);
				}
				PostRender(RenderEventArgs(&g, cells, !fullInvalidation));
				invalidateCells.clear();
				fullInvalidation = false;
				EndPaint(hWnd, &ps);
			}
		}
		break;
	case WM_DESTROY:
		{
			if (data)
			{
				delete[] data;
			}
			if (mapImage)
			{
				delete mapImage;
			}
			if (map)
			{
				delete map;
			}
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

Gdiplus::Point MapPanel::MapToClient(Gdiplus::Point point)
{
	Gdiplus::Point pt = point;
	compositeTransform->TransformPoints(&pt);
	return pt;
}

Gdiplus::Size MapPanel::MapToClient(Gdiplus::Size size)
{
	Gdiplus::Point pt = size;
	Gdiplus::REAL m[6];
	compositeTransform->GetElements(m);
	compositeTransform->TransformVectors(&pt);
	return Gdiplus::Size(pt.X, pt.Y);
}

Gdiplus::Rect MapPanel::MapToClient(Gdiplus::Rect rectangle)
{
	Gdiplus::Point pt[2];
	rectangle.GetLocation(&pt[0]);
	pt[1] = Gdiplus::Point(rectangle.GetRight(), rectangle.GetBottom());
	compositeTransform->TransformPoints(pt, 2);
	return Gdiplus::Rect(pt[0], Gdiplus::Size(pt[1].X - pt[0].X, pt[1].Y - pt[0].Y));
}

Gdiplus::Point MapPanel::ClientToMap(Gdiplus::Point point)
{
	Gdiplus::Point pt = point;
	invCompositeTransform->TransformPoints(&pt);
	return pt;
}

Gdiplus::Point MapPanel::ClientToMap(Gdiplus::Size size)
{
	Gdiplus::Point pt = size;
	invCompositeTransform->TransformVectors(&pt);
	return pt;
}

Gdiplus::Rect MapPanel::ClientToMap(Gdiplus::Rect rectangle)
{
	Gdiplus::Point pt[2];
	rectangle.GetLocation(&pt[0]);
	pt[1] = Gdiplus::Point(rectangle.GetRight(), rectangle.GetBottom());
	invCompositeTransform->TransformPoints(pt, 2);
	return Gdiplus::Rect(pt[0], Gdiplus::Size(pt[1].X - pt[0].X, pt[1].Y - pt[0].Y));
}

void MapPanel::UpdateCamera()
{
	if (mapImage == nullptr || clientWidth == 0 || clientHeight == 0)
	{
		return;
	}
	updatingCamera = true;
	double num = static_cast<double>(mapImage->GetWidth()) / static_cast<double>(mapImage->GetHeight());
	double num2 = static_cast<double>(clientWidth) / static_cast<double>(clientHeight);
	Gdiplus::Point location;
	cameraBounds.GetLocation(&location);
	Gdiplus::Size empty(0, 0);
	if (num2 > num)
	{
		empty.Height = mapImage->GetHeight() / zoom;
		empty.Width = static_cast<int>(static_cast<double>(empty.Height) * num2);
	}
	else
	{
		empty.Width = mapImage->GetWidth() / zoom;
		empty.Height = static_cast<int>(static_cast<double>(empty.Width) / num2);
	}
	Gdiplus::Point empty2(0, 0);
	Gdiplus::Size empty3(0, 0);
	if (empty.Width < static_cast<INT>(mapImage->GetWidth()))
	{
		empty2.X = max(0, min(static_cast<INT>(mapImage->GetWidth()) - empty.Width, cameraBounds.GetLeft()));
		empty3.Width = static_cast<INT>(mapImage->GetWidth()) * clientWidth / empty.Width;
	}
	else
	{
		empty2.X = (static_cast<INT>(mapImage->GetWidth()) - empty.Width) / 2;
	}
	if (empty.Height < static_cast<INT>(mapImage->GetHeight()))
	{
		empty2.Y = max(0, min(static_cast<INT>(mapImage->GetHeight()) - empty.Height, cameraBounds.GetTop()));
		empty3.Height = static_cast<INT>(mapImage->GetHeight()) * clientHeight / empty.Height;
	}
	else
	{
		empty2.Y = (static_cast<INT>(mapImage->GetHeight()) - empty.Height) / 2;
	}
	cameraBounds = Gdiplus::Rect(empty2, empty);
	RecalculateTransforms();
	if (referencePositionsSet)
	{
		Gdiplus::Point item = referencePositionsMap;
		Gdiplus::SizeF item2 = referencePositionsClient;
		cameraBounds.GetLocation(&location);
		if (empty3.Width != 0)
		{
			location.X = max(0, min(static_cast<INT>(mapImage->GetWidth()) - cameraBounds.Width, (int)(static_cast<float>(item.X) - static_cast<float>(cameraBounds.Width) * item2.Width)));
		}
		if (empty3.Height != 0)
		{
			location.Y = max(0, min(static_cast<INT>(mapImage->GetHeight()) - cameraBounds.Height, (int)(static_cast<float>(item.Y) - static_cast<float>(cameraBounds.Height) * item2.Height)));
		}
		if (empty3.Width || empty3.Height)
		{
			cameraBounds.X = location.X;
			cameraBounds.Y = location.Y;
			RecalculateTransforms();
		}
		referencePositionsSet = false;
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
	SetScrollPos(window, SB_HORZ, -AutoScrollPosition.X, true);
	SetScrollPos(window, SB_VERT, -AutoScrollPosition.Y, true);
	ResumeDrawing();
	updatingCamera = false;
	Invalidate();
}

void MapPanel::RecalculateTransforms()
{
	mapToViewTransform->Reset();
	mapToViewTransform->Translate(static_cast<float>(cameraBounds.GetLeft()), static_cast<float>(cameraBounds.GetTop()));
	mapToViewTransform->Scale(static_cast<float>(cameraBounds.Width), static_cast<float>(cameraBounds.Height));
	mapToViewTransform->Invert();
	Gdiplus::REAL m[6];
	mapToViewTransform->GetElements(m);
	viewToPageTransform->Reset();
	viewToPageTransform->Scale(static_cast<float>(clientWidth), static_cast<float>(clientHeight));
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

void MapPanel::InvalidateScroll()
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
			location.X = max(0, min(static_cast<INT>(mapImage->GetWidth()) - cameraBounds.Width, cameraBounds.GetLeft() + size.Width));
		}
		if (AutoScrollMinSize.Height != 0)
		{
			location.Y = max(0, min(static_cast<INT>(mapImage->GetHeight()) - cameraBounds.Height, cameraBounds.GetTop() + size.Height));
		}
		if (AutoScrollMinSize.Width && AutoScrollMinSize.Height)
		{
			cameraBounds.X = location.X;
			cameraBounds.Y = location.Y;
			RecalculateTransforms();
		}
		Invalidate();
	}
}

void MapPanel::UpdateScrollBars()
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
		ShowScrollBar(window, SB_HORZ, needhscroll);
		ShowScrollBar(window, SB_VERT, needvscroll);
		SetWindowPos(window, nullptr, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
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
			SetScrollInfo(window, SB_HORZ, &si, TRUE);
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
			SetScrollInfo(window, SB_VERT, &si, TRUE);
		}
		UpdateCamera();
	}
}

void MapPanel::SuspendDrawing()
{
	SendMessage(window, WM_SETREDRAW, FALSE, 0);
}

void MapPanel::ResumeDrawing()
{
	SendMessage(window, WM_SETREDRAW, TRUE, 0);
}

void MapPanel::SetZoom(int value)
{
	int num = max(minZoom, min(maxZoom, value));
	if (zoom != num)
	{
		zoom = num;
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(window, &pt);
		Gdiplus::Point point;
		point.X = pt.x;
		point.Y = pt.y;
		referencePositionsMap = ClientToMap(point);
		referencePositionsClient = Gdiplus::SizeF(static_cast<float>(point.X) / static_cast<float>(clientWidth), static_cast<float>(point.Y) / static_cast<float>(clientHeight));
		referencePositionsSet = true;
		UpdateCamera();
	}
}

void MapPanel::SetZoomStep(int value)
{
	if (zoomStep != value)
	{
		zoomStep = value;
		SetZoom(zoom / zoomStep * zoomStep);
	}
}

void MapPanel::SetMinZoom(int value)
{
	if (minZoom != value)
	{
		minZoom = value;
		SetZoom(zoom);
	}
}

void MapPanel::SetMaxZoom(int value)
{
	if (maxZoom != value)
	{
		maxZoom = value;
		SetZoom(zoom);
	}
}

int MapPanel::GetScrollPosition(int bar, UINT code)
{
	SCROLLINFO si = {};
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
	GetScrollInfo(window, bar, &si);
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
		result = max(si.nPos - static_cast<int>(si.nPage), minPos);
		break;
	case SB_PAGEDOWN:
		result = min(si.nPos + static_cast<int>(si.nPage), maxPos);
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

void MapPanel::FileOpen(const char* fname)
{
	if (data)
	{
		delete[] data;
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
	UpdateCamera();
	Invalidate(map);
}

void MapPanel::Invalidate()
{
	if (AutoScrollMinSize.Width && AutoScrollMinSize.Height)
	{
		InvalidateRect(window, nullptr, FALSE);
	}
	else
	{
		InvalidateRect(window, nullptr, TRUE);
	}
}

void MapPanel::Invalidate(Map*)
{
	if (!fullInvalidation)
	{
		invalidateCells.clear();
		fullInvalidation = true;
		Invalidate();
	}
}

void MapPanel::Invalidate(Map* invalidateMap, Gdiplus::Rect cellBounds)
{
	if (!fullInvalidation)
	{
		unsigned int count = invalidateCells.size();
		auto set = GetPoints(cellBounds);
		UnionWith(invalidateCells, set);
		if (invalidateCells.size() > count)
		{
			std::set<Gdiplus::Point> other = invalidateMap->overlappers->Overlaps(invalidateCells);
			UnionWith(invalidateCells, other);
		}
	}
}

void MapPanel::Invalidate(Map* invalidateMap, std::vector<Gdiplus::Rect> cellBounds)
{
	if (!fullInvalidation)
	{
		unsigned int count = invalidateCells.size();
		auto s = from(cellBounds).selectMany([](Gdiplus::Rect c) {return from(GetPoints(c)); }).toStdSet();
		UnionWith(invalidateCells,s);
		if (invalidateCells.size() > count)
		{
			auto other = invalidateMap->overlappers->Overlaps(invalidateCells);
			UnionWith(invalidateCells, other);
			Invalidate();
		}
	}
}

void MapPanel::Invalidate(Map* invalidateMap, Gdiplus::Point location)
{
	if (!fullInvalidation)
	{
		Invalidate(invalidateMap, Gdiplus::Rect(location, Gdiplus::Size(1, 1)));
	}
}

void MapPanel::Invalidate(Map* invalidateMap, std::vector<Gdiplus::Point> locations)
{
	if (!fullInvalidation)
	{
		auto s = from(locations).select([](Gdiplus::Point l) {return Gdiplus::Rect(l, Gdiplus::Size(1, 1)); }).toStdVector();
		Invalidate(invalidateMap, s);
	}
}

void MapPanel::Invalidate(Map* invalidateMap, int cell)
{
	Gdiplus::Point location;
	if (!fullInvalidation && invalidateMap->metrics->GetLocation(cell, location))
	{
		Invalidate(invalidateMap, location);
	}
}

void MapPanel::Invalidate(Map* invalidateMap, std::vector<int> cells)
{
	if (!fullInvalidation)
	{
		auto s = from(cells).where([invalidateMap](int c) {Gdiplus::Point p; return invalidateMap->metrics->GetLocation(c, p); }).select([invalidateMap](int c) {Gdiplus::Point location; invalidateMap->metrics->GetLocation(c, location); return location; }).toStdVector();
		Invalidate(invalidateMap, s);
	}
}

void MapPanel::Invalidate(Map* invalidateMap, Overlapper* overlapper)
{
	if (!fullInvalidation)
	{
		Gdiplus::Rect r;
		if (invalidateMap->overlappers->GetRect(overlapper, r))
		{
			Invalidate(invalidateMap, r);
		}
	}
}

void MapPanel::PreRender(RenderEventArgs e)
{
	if (!e.HasCells || e.Cells.size())
	{
		Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(mapImage);
		auto s = from(e.Cells).where([&](Gdiplus::Point p) {return map->metrics->Contains(p); }).toStdSet();
		Invalidate(map);
		MapRender::Render(map, g, s, MAPLAYER_ALL);
		delete g;
	}
}

void MapPanel::PostRender(RenderEventArgs e)
{
}

void MapPanel::OnMouseMove(Gdiplus::Point p)
{
	if (!map)
	{
		return;
	}
	Gdiplus::Point point = ClientToMap(p);
	Gdiplus::Point location(static_cast<int>(floor(static_cast<double>(point.X) / static_cast<double>(TileWidth))), static_cast<int>(floor(static_cast<double>(point.Y) / static_cast<double>(TileHeight))));
	int cell;
	if (map->metrics->GetCell(location, cell))
	{
		std::stringstream s;
		s << "X = " << location.X << ", Y = " << location.Y << ", Cell = " << cell;
		Template* t = map->templates->Get(cell);
		if (t && t->type)
		{
			s << ", Template = " << t->type->Name << " (" << t->icon << ")";
		}
		Smudge* sm = map->smudges->Get(cell);
		if (sm && sm->type)
		{
			s << ", Smudge = " << sm->type->Name;
		}
		Overlay* o = map->overlays->Get(cell);
		if (o && o->type)
		{
			s << ", Overlay = " << TheTextManager->Get(o->type->TextId.c_str());
		}
		Occupier *oc = map->technos->Get(location);
		if (oc)
		{
			Terrain* tr = dynamic_cast<Terrain*>(oc);
			{
				if (tr && tr->type)
				{
					s << ", Terrain = " << tr->type->Name.c_str();
				}
			}
			InfantryGroup* i = dynamic_cast<InfantryGroup*>(oc);
			{
				if (i)
				{
					int num = InfantryGroup::ClosestStoppingTypes(Gdiplus::Point(point.X * 24 / TileWidth % 24, point.Y * 24 / TileHeight % 24))[0];
					if (i->infantry[num])
					{
						s << ", Infantry = " << TheTextManager->Get(i->infantry[num]->type->TextId.c_str());
					}
				}
			}
			Unit* u = dynamic_cast<Unit*>(oc);
			{
				if (u && u->type)
				{
					s << ", Unit = " << TheTextManager->Get(u->type->TextId.c_str());
				}
			}
			Vessel* v = dynamic_cast<Vessel*>(oc);
			{
				if (v && v->type)
				{
					s << ", Vessel = " << TheTextManager->Get(v->type->TextId.c_str());
				}
			}
			Aircraft* a = dynamic_cast<Aircraft*>(oc);
			{
				if (a && a->type)
				{
					s << ", Aircraft = " << TheTextManager->Get(a->type->TextId.c_str());
				}
			}
			Structure* st = dynamic_cast<Structure*>(oc);
			{
				if (st && st->type)
				{
					s << ", Structure = " << TheTextManager->Get(st->type->TextId.c_str());
				}
			}
		}
		MainWindow::mainwindow->statusbar->SetText(1, s.str().c_str());
	}
	else
	{
		MainWindow::mainwindow->statusbar->SetText(1, "");
	}
}
