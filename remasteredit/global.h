#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlwapi.h>
#pragma warning(disable: 4458)
#include <gdiplus.h>
#pragma warning(default: 4458)
#include <string>
#include <map>
#include <vector>
#include <set>
constexpr int TileScale = 2;
constexpr int TileWidth = 128 / TileScale;
constexpr int TileHeight = 128 / TileScale;
enum MapLayerFlag
{
	MAPLAYER_NONE = 0x0,
	MAPLAYER_BASIC = 0x1,
	MAPLAYER_MAP = 0x2,
	MAPLAYER_TEMPLATE = 0x4,
	MAPLAYER_TERRAIN = 0x8,
	MAPLAYER_RESOURCES = 0x10,
	MAPLAYER_WALLS = 0x20,
	MAPLAYER_OVERLAY = 0x40,
	MAPLAYER_SMUDGE = 0x80,
	MAPLAYER_WAYPOINTS = 0x100,
	MAPLAYER_CELLTRIGGERS = 0x200,
	MAPLAYER_HOUSES = 0x400,
	MAPLAYER_INFANTRY = 0x800,
	MAPLAYER_UNITS = 0x1000,
	MAPLAYER_BUILDINGS = 0x2000,
	MAPLAYER_BOUNDARIES = 0x4000,
	MAPLAYER_TECHNOTRIGGERS = 0x8000,
	MAPLAYER_OVERLAYALL = 0x70,
	MAPLAYER_TECHNOS = 0x3828,
	MAPLAYER_ALL = 2147483647
};
enum InfantryStoppingType
{
	STOPPING_CENTER,
	STOPPING_UPPERLEFT,
	STOPPING_UPPERRIGHT,
	STOPPING_LOWERLEFT,
	STOPPING_LOWERRIGHT
};
namespace std {
	template<>
	struct less<Gdiplus::Point>
	{
		bool operator()(const Gdiplus::Point& p1, const Gdiplus::Point& p2) const
		{
			return (p1.Y * 128 + p1.X) < (p2.Y * 128 + p2.X);
		}
	};
}
namespace std {
	template<>
	struct less<Gdiplus::Rect>
	{
		bool operator()(const Gdiplus::Rect& p1, const Gdiplus::Rect& p2) const
		{
			return (p1.Y * 128 + p1.X + 256 * p1.Width + 512 * p1.Height) < (p2.Y * 128 + p2.X + 256 * p2.Width + 512 * p2.Height);
		}
	};
}
