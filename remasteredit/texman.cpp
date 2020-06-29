#include "texman.h"
#include "miniz.h"
#include "ramfile.h"
#include "teamcolor.h"
#include "megfilemanager.h"
#include "cellmetrics.h"
#include "WWFILE.H"
#include <algorithm>
#define FREEIMAGE_LIB
#include "FreeImage.h"
#include "json.hpp"
using json = nlohmann::json;
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

Gdiplus::Rect TextureManager::CalculateOpaqueBounds(unsigned char* data, int width, int height, int bpp, int stride)
{
	Gdiplus::Rect opaqueBounds(0, 0, width, height);
	for (int i = 0; i < height; i++)
	{
		if (!isTransparentRow(i, data, bpp, stride))
		{
			opaqueBounds.Offset(0, i);
			break;
		}
	}
	for (int num = height; num > 0; num--)
	{
		if (!isTransparentRow(num - 1, data, bpp, stride))
		{
			opaqueBounds.Height = num - opaqueBounds.Y;
			break;
		}
	}
	for (int j = 0; j < width; j++)
	{
		if (!isTransparentColumn(j, data, bpp, stride, opaqueBounds))
		{
			opaqueBounds.Offset(j, 0);
			break;
		}
	}
	for (int num2 = width; num2 > 0; num2--)
	{
		if (!isTransparentColumn(num2 - 1, data, bpp, stride, opaqueBounds))
		{
			opaqueBounds.Width = num2 - opaqueBounds.X;
			break;
		}
	}
	return opaqueBounds;
}

bool TextureManager::isTransparentColumn(int x, unsigned char* data, int bpp, int stride, Gdiplus::Rect opaqueBounds)
{
	int num3 = x * bpp + (bpp - 1);
	for (int k = opaqueBounds.Y; k < opaqueBounds.Y + opaqueBounds.Height; k++)
	{
		if (data[num3 + k * stride] != 0)
		{
			return false;
		}
	}
	return true;
}

bool TextureManager::isTransparentRow(int y, unsigned char* data, int bpp, int stride)
{
	int num4 = y * stride;
	for (int l = bpp - 1; l < stride; l += bpp)
	{
		if (data[num4 + l] != 0)
		{
			return false;
		}
	}
	return true;
}

Gdiplus::Rect TextureManager::CalculateOpaqueBounds(Gdiplus::Bitmap *bitmap)
{
	Gdiplus::BitmapData bitmapData;
	Gdiplus::Rect r(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
	bitmap->LockBits(&r, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, bitmap->GetPixelFormat(), &bitmapData);
	int bpp = Gdiplus::GetPixelFormatSize(bitmapData.PixelFormat) / 8;
	Gdiplus::Rect ret = CalculateOpaqueBounds((unsigned char *)bitmapData.Scan0, bitmapData.Width, bitmapData.Height, bpp, bitmapData.Stride);
	bitmap->UnlockBits(&bitmapData);
	return ret;
}

Gdiplus::Bitmap *CopyBitmap(Gdiplus::Bitmap *bitmap)
{
	int width = bitmap->GetWidth();
	int height = bitmap->GetHeight();
	Gdiplus::Bitmap* bm = new Gdiplus::Bitmap(width, height);
	Gdiplus::Graphics *g = Gdiplus::Graphics::FromImage(bm);
	g->Clear(Gdiplus::Color::Transparent);
	g->DrawImage(bitmap, 0, 0, width, height);
	delete g;
	return bm;
}

std::pair<Gdiplus::Bitmap*, Gdiplus::Rect> TextureManager::GetTexture(const char* filename, TeamColor* color)
{
	std::pair<Gdiplus::Bitmap*, Gdiplus::Rect> value;
	std::string name;
	if (color)
	{
		name = color->Name;
	}
	if (TryGetValue(TeamColorTextures, std::make_pair<std::string, std::string>(std::string(filename), std::string(name)), value))
	{
		return value;
	}
	if (!TryGetValue(CachedTextures, filename, value.first))
	{
		if (!_stricmp(PathFindExtension(filename), ".tga"))
		{
			FileClass *tga = nullptr;
			char* jstr = nullptr;
			char path[MAX_PATH];
			strcpy(path, filename);
			PathRemoveFileSpec(path);
			strcat(path, ".zip");
			FileClass* zip = m->Open(path);
			if (zip)
			{
				int size = zip->Size();
				void* data = new char[size];
				zip->Read(data, size);
				mz_zip_archive zip_archive;
				memset(&zip_archive, 0, sizeof(zip_archive));
				mz_zip_reader_init_mem(&zip_archive, data, size, 0);
				char* fname = PathFindFileName(filename);
				char fn[MAX_PATH];
				strcpy(fn, fname);
				size_t sz;
				void *buf = mz_zip_reader_extract_file_to_heap(&zip_archive, fn, &sz, 0);
				if (buf)
				{
					RAMFileClass *r = new RAMFileClass(buf, sz, true);
					r->Open();
					tga = r;
					free(buf);
				}
				PathRenameExtension(fn, ".meta");
				void *buf2 = mz_zip_reader_extract_file_to_heap(&zip_archive, fn, &sz, 0);
				if (buf2)
				{
					jstr = new char[sz+1];
					memcpy(jstr, buf2, sz);
					jstr[sz] = 0;
					free(buf2);
				}
				mz_zip_reader_end(&zip_archive);
				delete[] data;
				delete zip;
			}
			if (tga == nullptr)
			{
				FileClass* f = m->Open(path);
				if (f)
				{
					tga = f;
				}
			}
			if (tga)
			{
				FreeImageIO io;
				io.read_proc = myReadProc;
				io.write_proc = myWriteProc;
				io.seek_proc = mySeekProc;
				io.tell_proc = myTellProc;
				FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromHandle(&io, (fi_handle)tga, 0);
				if (fif != FIF_UNKNOWN)
				{
					FIBITMAP* dib = FreeImage_LoadFromHandle(fif, &io, (fi_handle)tga, 0);
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
					std::string fname = "test";
					fname += ".png";
					wchar_t p[MAX_PATH];
					mbstowcs(p, fname.c_str(), MAX_PATH);
					CLSID pngClsid;
					int GetEncoderClsid(const WCHAR * format, CLSID * pClsid);
					GetEncoderClsid(L"image/png", &pngClsid);
					bitmap->Save(p, &pngClsid);
					FreeImage_Unload(dib);
					if (jstr)
					{
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
						delete[] jstr;
						delete bitmap;
						bitmap = bitmap2;
					}
					CachedTextures[filename] = bitmap;
				}
				delete tga;
			}
		}
		if (!TryGetValue(CachedTextures, filename, value.first))
		{
			char* fn = _strdup(filename);
			PathRenameExtension(fn, ".dds");
			FileClass* f = m->Open(fn);
			if (f)
			{
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
					CachedTextures[filename] = bitmap;
				}
				delete f;
			}
			free(fn);
		}
	}
	if (!TryGetValue(CachedTextures, filename, value.first))
	{
		return value;
	}
	value.first = CopyBitmap(value.first);
	value.second = Gdiplus::Rect(0, 0, value.first->GetWidth(), value.first->GetHeight());
	if (color != nullptr)
	{
		DebugBreak();
	}
	else
	{
		value.second = CalculateOpaqueBounds(value.first);
	}
	TeamColorTextures[std::make_pair<std::string, std::string>(std::string(filename), std::string(name))] = value;
	return value;
}

TextureManager::~TextureManager()
{
	for (auto i : CachedTextures)
	{
		delete i.second;
	}
	for (auto i : TeamColorTextures)
	{
		delete i.second.first;
	}
}

TextureManager* TheTextureManager;
void InitTextures()
{
	TheTextureManager = new TextureManager(TheMegFileManager);
}

void ShutdownTextures()
{
	delete TheTextureManager;
}
