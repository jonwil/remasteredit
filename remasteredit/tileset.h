#pragma once
class TeamColor;
class TextureManager;
class MegFileManager;
class Tile
{
public:
	Gdiplus::Bitmap* Image;
	Gdiplus::Rect OpaqueBounds;
};

class TileData
{
public:
	int FPS;
	int FrameCount;
	std::string* Frames;
	std::map<std::string, std::pair<Tile*, int>> TeamColorTiles;
	TileData();
	~TileData();
};

class Tileset
{
private:
	TextureManager* textureManager;
	MegFileManager* m;
	std::map<std::string, std::map<int, TileData*>> tiles;
	std::string tilesetname;
public:
	Tileset(MegFileManager *manager, TextureManager *t) : m(manager), textureManager(t)
	{
	}
	void Load(const char* path, const char* tpath);
	void Reset();
	bool GetTileData(const char* name, int shape, TeamColor* teamColor, int& fps, std::pair<Tile*, int>& outtiles);
	const char* GetName() { return tilesetname.c_str(); }
	~Tileset();
};

class TilesetManager
{
private:
	MegFileManager* m;
	std::map<std::string, Tileset*> tilesets;
	const char** searchTilesets;
public:
	TilesetManager(MegFileManager* manager, TextureManager *textureManager, const char* xmlpath, const char* texturespath);
	void Reset();
	bool GetTeamColorTileData(const char *name, int shape, TeamColor *teamColor, int& fps, std::pair<Tile*, int>& tiles);
	bool GetTeamColorTileData(const char* name, int shape, TeamColor *teamColor, int& fps, Tile *& tile);
	bool GetTeamColorTileData(const char* name, int shape, TeamColor *teamColor, Tile*& tile);
	bool GetTileData(const char* name, int shape, Tile*& tile);
	void SetSets(const char** newsets) { searchTilesets = newsets; }
	~TilesetManager();
};

extern TilesetManager* TheTilesetManager;
void InitTilesets();
void ShutdownTilesets();
