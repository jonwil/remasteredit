#pragma once
#include <map>
#include <string>
#include <Shlwapi.h>
#pragma warning(disable: 4458)
#include <gdiplus.h>
#pragma warning(default: 4458)
class MegFileManager;
class TeamColor;
class TextureManager
{
private:
	MegFileManager* m;
	std::map<std::string, Gdiplus::Bitmap*> CachedTextures;
	std::map<std::pair<std::string, std::string>, std::pair<Gdiplus::Bitmap*, Gdiplus::Rect>> TeamColorTextures;
	static Gdiplus::Rect CalculateOpaqueBounds(unsigned char* data, int width, int height, int bpp, int stride);
	static Gdiplus::Rect CalculateOpaqueBounds(Gdiplus::Bitmap *bitmap);
	static bool isTransparentColumn(int x, unsigned char* data, int bpp, int stride, Gdiplus::Rect opaqueBounds);
	static bool isTransparentRow(int y, unsigned char* data, int bpp, int stride);
public:
	TextureManager(MegFileManager* manager) : m(manager)
	{
	}
	~TextureManager();
	std::pair<Gdiplus::Bitmap*, Gdiplus::Rect> GetTexture(const char* filename, TeamColor* color);
};

extern TextureManager* TheTextureManager;
void InitTextures();
void ShutdownTextures();
