#include <Shlwapi.h>
#include <algorithm>
#pragma warning(disable: 4458)
#include <gdiplus.h>
#pragma warning(default: 4458)
#define FREEIMAGE_LIB
#include "FreeImage.h"
#include "json.hpp"
using json = nlohmann::json;
#include "tileset.h"
#include "pugixml.hpp"
#include "wwfile.h"

unsigned DLL_CALLCONV myReadProc(void* buffer, unsigned size, unsigned count, fi_handle handle)
{
	FileClass* f = (FileClass*)handle;
	return f->Read(buffer, size * count);
}

unsigned DLL_CALLCONV myWriteProc(void* buffer, unsigned size, unsigned count, fi_handle handle)
{
	FileClass* f = (FileClass*)handle;
	return f->Write(buffer, size * count);
}

int DLL_CALLCONV mySeekProc(fi_handle handle, long offset, int origin)
{
	FileClass* f = (FileClass*)handle;
	return f->Seek(offset, origin);
}

long DLL_CALLCONV myTellProc(fi_handle handle)
{
	FileClass* f = (FileClass*)handle;
	return f->Seek(0, SEEK_CUR);
}

int GetPixelFormat(FIBITMAP* dib)
{
	int result = 0;

	if (FreeImage_GetImageType(dib) == FIT_BITMAP)
	{
		switch (FreeImage_GetBPP(dib))
		{
		case 1:
			result = PixelFormat1bppIndexed;
			break;
		case 4:
			result = PixelFormat4bppIndexed;
			break;
		case 8:
			result = PixelFormat8bppIndexed;
			break;
		case 16:
			if ((FreeImage_GetBlueMask(dib) == FI16_565_BLUE_MASK) &&
				(FreeImage_GetGreenMask(dib) == FI16_565_GREEN_MASK) &&
				(FreeImage_GetRedMask(dib) == FI16_565_RED_MASK))
			{
				result = PixelFormat16bppRGB565;
			}
			if ((FreeImage_GetBlueMask(dib) == FI16_555_BLUE_MASK) &&
				(FreeImage_GetGreenMask(dib) == FI16_555_GREEN_MASK) &&
				(FreeImage_GetRedMask(dib) == FI16_555_RED_MASK))
			{
				result = PixelFormat16bppRGB555;
			}
			break;
		case 24:
			result = PixelFormat24bppRGB;
			break;
		case 32:
			result = PixelFormat32bppARGB;
			break;
		}
	}
	return result;
}

Tile::Tile() : Image(nullptr), OpaqueBounds{}
{
}

Tile::~Tile()
{
	delete Image;
}

TileData::TileData() : FPS(0), FrameCount(0), Frames(nullptr)
{
}

TileData::~TileData()
{
	delete[] Frames;
	for (auto i : Tiles)
	{
		delete i.second;
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
			if (!m->Exists(file.c_str()))
			{
				file[file.length() - 3] = 'd';
				file[file.length() - 2] = 'd';
				file[file.length() - 1] = 's';
			}
			d->Frames[i] = file;
		}
		char* p = _strdup(name);
		_strupr(p);
		tiles[p][shape] = d;
		delete[] p;
	}
}

int Tileset::GetTileFPS(const char* name, int shape)
{
	char* p = _strdup(name);
	_strupr(p);
	int fps = tiles[p][shape]->FPS;
	delete[] p;
	return fps;
}

int Tileset::GetTileFrames(const char* name, int shape)
{
	char* p = _strdup(name);
	_strupr(p);
	int count = tiles[p][shape]->FrameCount;
	delete[] p;
	return count;
}

const char* Tileset::GetTileName(const char* name, int shape, int frame)
{
	char* p = _strdup(name);
	_strupr(p);
	const char* n = tiles[p][shape]->Frames[frame].c_str();
	delete[] p;
	return n;
}

Tile* Tileset::GetTileImage(const char* name, int shape, int frame)
{
	char* p = _strdup(name);
	_strupr(p);
	const char* n = tiles[p][shape]->Frames[frame].c_str();
	if (!tiles[p][shape]->Tiles.count(n))
	{
		FileClass* f = m->Open(n);
		FreeImageIO io;
		io.read_proc = myReadProc;
		io.write_proc = myWriteProc;
		io.seek_proc = mySeekProc;
		io.tell_proc = myTellProc;
		FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromHandle(&io, (fi_handle)f, 0);
		if (fif != FIF_UNKNOWN)
		{
			FIBITMAP* dib = FreeImage_LoadFromHandle(fif, &io, (fi_handle)f, 0);
			FREE_IMAGE_TYPE type = FreeImage_GetImageType(dib);
			if (type != FIT_BITMAP)
			{
				FIBITMAP* old = dib;
				dib = FreeImage_ConvertToType(dib, FIT_BITMAP);
				FreeImage_Unload(old);
			}
			int height = FreeImage_GetHeight(dib);
			int width = FreeImage_GetWidth(dib);
			int pitch = FreeImage_GetPitch(dib);
			int format = GetPixelFormat(dib);
			Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(width, height, format);
			Gdiplus::BitmapData data;
			Gdiplus::Rect rect(0, 0, width, height);
			bitmap->LockBits(&rect, Gdiplus::ImageLockModeWrite, format, &data);
			FreeImage_ConvertToRawBits((BYTE*)data.Scan0, dib, pitch, FreeImage_GetBPP(dib), FreeImage_GetRedMask(dib), FreeImage_GetGreenMask(dib), FreeImage_GetBlueMask(dib), true);
			bitmap->UnlockBits(&data);
			FreeImage_Unload(dib);
			if (!_stricmp(PathFindExtension(n), ".tga"))
			{
				std::string s = n;
				s += ".meta";
				if (m->Exists(s.c_str()))
				{
					FileClass* f2 = m->Open(s.c_str());
					char* jstr = new char[f2->Size()+1];
					jstr[f2->Size()] = 0;
					f2->Read(jstr, f2->Size());
					m->Close(f2);
					json j = json::parse(jstr);
					int jwidth = j["size"][0].get<int>();
					int jheight = j["size"][1].get<int>();
					Gdiplus::Rect r;
					int left = j["crop"][0].get<int>();
					int top = j["crop"][1].get<int>();
					int right = j["crop"][2].get<int>();
					int bottom = j["crop"][3].get<int>();
					r.X = left;
					r.Y = top;
					r.Width = right - left;
					r.Height = bottom - top;
					Gdiplus::Bitmap* bitmap2 = new Gdiplus::Bitmap(jwidth, jheight, bitmap->GetPixelFormat());
					Gdiplus::Graphics* graphics = Gdiplus::Graphics::FromImage(bitmap2);
					graphics->DrawImage(bitmap, r, 0, 0, bitmap->GetWidth(), bitmap->GetHeight(), Gdiplus::UnitPixel);
					delete jstr;
					delete bitmap;
					bitmap = bitmap2;
				}
			}
			Tile* t = new Tile;
			t->Image = bitmap;
			RECT r;
			r.top = 0;
			r.bottom = bitmap->GetHeight();
			r.right = 0;
			r.left = bitmap->GetWidth();
			t->OpaqueBounds = r;
			tiles[p][shape]->Tiles[n] = t;
		}
		m->Close(f);
	}
	Tile* t = tiles[p][shape]->Tiles[n];
	delete[] p;
	return t;
}

bool Tileset::HasTile(const char* name, int shape)
{
	char* p = _strdup(name);
	_strupr(p);
	if (tiles.count(p))
	{
		if (tiles[p].count(shape))
		{
			delete[] p;
			return true;
		}
	}
	delete[] p;
	return false;
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

TilesetManager::TilesetManager(MegFileManager* manager, const char* xmlpath, const char* texturespath) : m(manager), sets(nullptr)
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
		Tileset* tileset = new Tileset(manager);
		tileset->Load(path.c_str(), texturespath);
		tilesets[tileset->GetName()] = tileset;
	}
}

int TilesetManager::GetTileFPS(const char* name, int shape)
{
	if (sets)
	{
		const char** sets2 = sets;
		while (*sets2)
		{
			if (tilesets.count(*sets2))
			{
				if (tilesets[*sets2]->HasTile(name, shape))
				{
					return tilesets[*sets2]->GetTileFPS(name, shape);
				}
			}
			sets2++;
		}
	}
	return 0;
}

int TilesetManager::GetTileFrames(const char* name, int shape)
{
	if (sets)
	{
		const char** sets2 = sets;
		while (*sets2)
		{
			if (tilesets.count(*sets2))
			{
				if (tilesets[*sets2]->HasTile(name, shape))
				{
					return tilesets[*sets2]->GetTileFrames(name, shape);
				}
			}
			sets2++;
		}
	}
	return 0;
}

const char* TilesetManager::GetTileName(const char* name, int shape, int frame)
{
	if (sets)
	{
		const char** sets2 = sets;
		while (*sets2)
		{
			if (tilesets.count(*sets2))
			{
				if (tilesets[*sets2]->HasTile(name, shape))
				{
					if (frame <= tilesets[*sets2]->GetTileFrames(name, shape))
					{
						return tilesets[*sets2]->GetTileName(name, shape, frame);
					}
				}
			}
			sets2++;
		}
	}
	return "";
}

Tile* TilesetManager::GetTileImage(const char* name, int shape, int frame)
{
	if (sets)
	{
		const char** sets2 = sets;
		while (*sets2)
		{
			if (tilesets.count(*sets2))
			{
				if (tilesets[*sets2]->HasTile(name, shape))
				{
					if (frame <= tilesets[*sets2]->GetTileFrames(name, shape))
					{
						return tilesets[*sets2]->GetTileImage(name, shape, frame);
					}
				}
			}
			sets2++;
		}
	}
	return nullptr;
}

TilesetManager::~TilesetManager()
{
	for (auto i : tilesets)
	{
		delete i.second;
	}
}

TilesetManager* TheTilesetManager;
void InitTilesets()
{
	TheTilesetManager = new TilesetManager(TheMegFileManager, "DATA\\XML\\TILESETS.XML", "DATA\\ART\\TEXTURES\\SRGB");
}

void ShutdownTilesets()
{
	delete TheTilesetManager;
}
