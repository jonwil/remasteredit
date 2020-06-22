#pragma once
#include <vector>
#include <string>
#include <map>
#include "megfilemanager.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlwapi.h>
#pragma warning(disable: 4458)
#include <gdiplus.h>
#pragma warning(default: 4458)
class Tile
{
public:
	Gdiplus::Bitmap* Image;
	RECT OpaqueBounds;
	Tile();
	~Tile();
};

class TileData
{
public:
	int FPS;
	int FrameCount;
	std::string* Frames;
	std::map<std::string, Tile*> Tiles;
	TileData();
	~TileData();
};

class Tileset
{
private:
	MegFileManager* m;
	std::map<std::string, std::map<int, TileData*>> tiles;
	std::string tilesetname;
public:
	Tileset(MegFileManager* manager) : m(manager)
	{
	}
	void Load(const char* path, const char* tpath);
	int GetTileFPS(const char* name, int shape);
	int GetTileFrames(const char* name, int shape);
	const char* GetTileName(const char* name, int shape, int frame);
	Tile* GetTileImage(const char* name, int shape, int frame);
	bool HasTile(const char* name, int shape);
	const char* GetName() { return tilesetname.c_str(); }
	~Tileset();
};

class TilesetManager
{
private:
	MegFileManager* m;
	std::map<std::string, Tileset*> tilesets;
	const char** sets;
public:
	TilesetManager(MegFileManager* manager, const char* xmlpath, const char* texturespath);
	int GetTileFPS(const char* name, int shape);
	int GetTileFrames(const char* name, int shape);
	const char* GetTileName(const char* name, int shape, int frame);
	Tile* GetTileImage(const char* name, int shape, int frame);
	void SetSets(const char** newsets) { sets = newsets; }
	~TilesetManager();
};

extern TilesetManager* TheTilesetManager;
void InitTilesets();
void ShutdownTilesets();
