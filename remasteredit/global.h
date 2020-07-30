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
#include <functional>
#include <CommCtrl.h>
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
			return std::tie(p1.X, p1.Y) < std::tie(p2.X, p2.Y);
		}
	};
}
namespace std {
	template<>
	struct less<Gdiplus::Rect>
	{
		bool operator()(const Gdiplus::Rect& p1, const Gdiplus::Rect& p2) const
		{
			return std::tie(p1.X, p1.Y, p1.Width, p1.Height) < std::tie(p2.X, p2.Y, p2.Width, p2.Height);
		}
	};
}

template <class Key, class Value, class Compare, class Allocator> inline bool TryGetValue(std::map<Key, Value, Compare, Allocator> const& theMap, typename std::map<Key, Value, Compare, Allocator>::key_type const& key, typename std::map<Key, Value, Compare, Allocator>::mapped_type& value)
{
	auto it = theMap.find(key);
	if (it == theMap.end())
	{
		return false;
	}
	value = it->second;
	return true;
}

template <class c> inline bool Any(c const &container)
{
	return container.size() != 0;
}

template <class c, class f> inline bool Any(c const &container, f func)
{
	for (auto i : container)
	{
		if (func(i))
		{
			return true;
		}
	}
	return false;
}

inline Gdiplus::Color FromArgb(BYTE alpha, Gdiplus::Color basecolor)
{
	return Gdiplus::Color(alpha, basecolor.GetR(), basecolor.GetG(), basecolor.GetB());
}

inline bool IsKeyDown(int key)
{
	if (GetKeyState(key) < 0)
	{
		return true;
	}
	return false;
}
