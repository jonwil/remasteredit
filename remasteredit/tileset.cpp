#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Shlwapi.h>
#include <algorithm>
#pragma warning(disable: 4458)
#include <gdiplus.h>
#pragma warning(default: 4458)
#include "pugixml.hpp"
#include "wwfile.h"
#include "tileset.h"
#include "teamcolor.h"
#include "texman.h"
#include "megfilemanager.h"
#include "cellmetrics.h"
static int tilescale = 2;
static int tilesize = 128 / tilescale;
Gdiplus::Bitmap* transparentTileImage;
TileData::TileData() : FPS(0), FrameCount(0), Frames(nullptr)
{
}

TileData::~TileData()
{
	delete[] Frames;
	for (auto i : TeamColorTiles)
	{
		delete[] i.second.first;
	}
}

void Tileset::Load(const char* path, const char* tpath)
{
	pugi::xml_document doc;
	FileClass* f = m->Open(path);
	int size = f->Size();
	char* buffer = new char[size];
	f->Read(buffer, size);
	doc.load_buffer(buffer, size);
	std::string texturepath = tpath;
	tilesetname = doc.select_node("Tilesets/TilesetTypeClass").node().attribute("name").value();
	pugi::xml_node texpath = doc.select_node("Tilesets/TilesetTypeClass/RootTexturePath").node();
	texturepath += "\\";
	texturepath += texpath.child_value();
	for (pugi::xpath_node n : doc.select_nodes("Tilesets/TilesetTypeClass/Tiles/Tile"))
	{
		pugi::xml_node item = n.node();
		TileData* d = new TileData;
		const char* name = item.select_node("Key/Name").node().child_value();
		int shape = atoi(item.select_node("Key/Shape").node().child_value());
		pugi::xpath_node node = item.select_node("Value/AnimationData/FPS");
		d->FPS = 0;
		if (node)
		{
			d->FPS = atoi(node.node().child_value());
		}
		pugi::xpath_node_set set = item.select_nodes("Value/Frames/Frame");
		d->FrameCount = set.size();
		d->Frames = new std::string[set.size()];
		for (unsigned int i = 0;i < set.size();i++)
		{
			std::string file = texturepath;
			file += "\\";
			file += set[i].node().child_value();
			d->Frames[i] = file;
		}
		char* p = _strdup(name);
		_strupr(p);
		tiles[p][shape] = d;
		delete[] p;
	}
}

template <class Key, class Value, class Compare, class Allocator> static int Max(std::map<Key, Value, Compare, Allocator> const& theMap, int(*func)(typename std::map<Key, Value, Compare, Allocator>::value_type const& a))
{
	int x = 0;
	for (const auto key : theMap)
	{
		int y = func(key);
		if (y > x)
		{
			x = y;
		}
	}
	return x;
}

bool Tileset::GetTileData(const char* name, int shape, TeamColor* teamColor, int& fps, std::pair<Tile*, int>& outtiles)
{
	char* c = _strdup(name);
	_strupr(c);
	teamColor;
	fps = 0;
	outtiles.first = nullptr;
	outtiles.second = 0;
	std::map<int, TileData*> value;
	if (!TryGetValue(tiles, c, value))
	{
		delete[] c;
		return false;
	}
	delete[] c;
	if (shape < 0)
	{
		shape = max(0, Max(value, [](auto a) {return a.first; }) + shape + 1);
	}
	TileData* value2;
	if (!TryGetValue(value, shape, value2))
	{
		return false;
	}
	std::string key;
	if (teamColor)
	{
		key = teamColor->Name;
	}
	std::pair<Tile*, int> value3;
	if (!TryGetValue(value2->TeamColorTiles, key, value3))
	{
		value3.first = new Tile[value2->FrameCount];
		value3.second = value2->FrameCount;
		value2->TeamColorTiles[key] = value3;
		for (int i = 0; i < value3.second; i++)
		{
			std::string text = value2->Frames[i];
			if (!text.empty())
			{
				auto texture = textureManager->GetTexture(text.c_str() , teamColor);
				value3.first[i].Image = texture.first;
				value3.first[i].OpaqueBounds = texture.second;
			}
			else
			{
				value3.first[i].Image = transparentTileImage;
				value3.first[i].OpaqueBounds = Gdiplus::Rect(0, 0, transparentTileImage->GetWidth(), transparentTileImage->GetHeight());
			}
		}
	}
	fps = value2->FPS;
	outtiles = value3;
	return true;
}

void Tileset::Reset()
{
	for (auto i : tiles)
	{
		for (auto j : i.second)
		{
			j.second->TeamColorTiles.clear();
		}
	}
}

Tileset::~Tileset()
{
	for (auto i : tiles)
	{
		for (auto j : i.second)
		{
			delete j.second;
		}
	}
}

TilesetManager::TilesetManager(MegFileManager* manager, TextureManager* textureManager, const char* xmlpath, const char* texturespath) : m(manager), searchTilesets(nullptr)
{
	pugi::xml_document doc;
	FileClass* f = manager->Open(xmlpath);
	int size = f->Size();
	char* buffer = new char[size];
	f->Read(buffer, size);
	doc.load_buffer(buffer, size);
	char* str = _strdup(xmlpath);
	strrchr(str, '\\')[0] = 0;
	for (pugi::xpath_node n : doc.select_nodes("TilesetFiles/File"))
	{
		std::string path = str;
		path += "\\";
		path += n.node().child_value();
		Tileset* tileset = new Tileset(manager, textureManager);
		tileset->Load(path.c_str(), texturespath);
		tilesets[tileset->GetName()] = tileset;
	}
}

TilesetManager::~TilesetManager()
{
	for (auto i : tilesets)
	{
		delete i.second;
	}
}

void TilesetManager::Reset()
{
	for (auto i : tilesets)
	{
		i.second->Reset();
	}
}

bool TilesetManager::GetTeamColorTileData(const char* name, int shape, TeamColor *teamColor, int& fps, std::pair<Tile*, int>& tiles)
{
	fps = 0;
	tiles.first = nullptr;
	tiles.second = 0;
	if (searchTilesets)
	{
		for (int i = 0; i < 5; i++)
		{
			if (tilesets.count(searchTilesets[i]))
			{
				if (tilesets[searchTilesets[i]]->GetTileData(name, shape, teamColor, fps, tiles))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool TilesetManager::GetTeamColorTileData(const char* name, int shape, TeamColor *teamColor, int& fps, Tile*& tile)
{
	tile = nullptr;
	std::pair<Tile*, int> tiles;
	if (!GetTeamColorTileData(name, shape, teamColor, fps, tiles))
	{
		return false;
	}
	tile = &tiles.first[0];
	return true;
}

bool TilesetManager::GetTeamColorTileData(const char* name, int shape, TeamColor *teamColor, Tile*& tile)
{
	int fps;
	return GetTeamColorTileData(name, shape, teamColor, fps, tile);
}

bool TilesetManager::GetTileData(const char* name, int shape, Tile*& tile)
{
	return GetTeamColorTileData(name, shape, nullptr, tile);
}

TilesetManager* TheTilesetManager;
void InitTilesets()
{
	TheTilesetManager = new TilesetManager(TheMegFileManager, TheTextureManager, "DATA\\XML\\TILESETS.XML", "DATA\\ART\\TEXTURES\\SRGB");
	transparentTileImage = new Gdiplus::Bitmap(tilesize, tilesize);
	Gdiplus::Graphics* graphics = Gdiplus::Graphics::FromImage(transparentTileImage);
	graphics->Clear(Gdiplus::Color::Transparent);
	delete graphics;
}

void ShutdownTilesets()
{
	delete TheTilesetManager;
	delete transparentTileImage;
}
