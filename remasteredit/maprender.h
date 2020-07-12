#pragma once
#include <functional>
class Map;
class Smudge;
class Overlay;
class Structure;
class Infantry;
using RenderFunc = std::function<void(Gdiplus::Graphics*,Gdiplus::Rect)>;
class MapRender
{
public:
	static void Render(Map* map, Gdiplus::Graphics* graphics, const std::set<Gdiplus::Point> &locations, MapLayerFlag layers, int tileScale);
	static void Render(Map* map, Gdiplus::Graphics* graphics, const std::set<Gdiplus::Point> &locations, MapLayerFlag layers);
	static std::pair<Gdiplus::Rect, RenderFunc> Render(Gdiplus::Point topLeft, Gdiplus::Size tileSize, int tileScale, Smudge* smudge);
	static std::pair<Gdiplus::Rect, RenderFunc> Render(bool isra, Gdiplus::Point topLeft, Gdiplus::Size tileSize, int tileScale, Overlay* overlay);
	static std::pair<Gdiplus::Rect, RenderFunc> Render(bool isra, Gdiplus::Point topLeft, Gdiplus::Size tileSize, int tileScale, Structure* building);
};
