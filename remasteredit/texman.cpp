#include "texman.h"
#include "miniz.h"
#include "ramfile.h"
#include "teamColor.h"
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

static float frac(float x)
{
	return x - (float)(int)x;
}
static float lerp(float x, float y, float t)
{
	return x * (1.0f - t) + y * t;
}
static float saturate(float x)
{
	return max(0, min(1, x));
}

static int GetPixelFormatSize(Gdiplus::PixelFormat pixfmt)
{
	return ((int)pixfmt >> 8) & 0xFF;
}

static float ToLinear(float v)
{
	if (!(v < 0.04045f))
	{
		return (float)pow((200 * v + 11.0f) / 211.0f, 2.4000000953674316f);
	}
	return v * 25.0f / 323.0f;
}

static float ToLinear(unsigned char v)
{
	return ToLinear((float)(int)v / 255.0f);
}

static float GetHue(Gdiplus::Color color)
{
	float R = color.GetR();
	float G = color.GetG();
	float B = color.GetB();
	if (R == G && G == B)
		return 0; // 0 makes as good an UNDEFINED value as any

	float r = (float)R / 255.0f;
	float g = (float)G / 255.0f;
	float b = (float)B / 255.0f;

	float max, min;
	float delta;
	float hue = 0.0f;

	max = r; min = r;

	if (g > max) max = g;
	if (b > max) max = b;

	if (g < min) min = g;
	if (b < min) min = b;

	delta = max - min;

	if (r == max) {
		hue = (g - b) / delta;
	}
	else if (g == max) {
		hue = 2 + (b - r) / delta;
	}
	else if (b == max) {
		hue = 4 + (r - g) / delta;
	}
	hue *= 60;

	if (hue < 0.0f) {
		hue += 360.0f;
	}
	return hue;
}

static float ToSRGB(float v)
{
	if (!((double)v < 0.0031308))
	{
		return ((float)pow(v, 0.4166666567325592) * 211 - 11) / 200;
	}
	return v * 323 / 25;
}

std::pair<Gdiplus::Bitmap*, Gdiplus::Rect> TextureManager::GetTexture(const char* filename, TeamColor* teamColor)
{
	std::pair<Gdiplus::Bitmap*, Gdiplus::Rect> value;
	std::string name;
	if (teamColor)
	{
		name = teamColor->Name;
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
	if (teamColor != nullptr)
	{
		Gdiplus::BitmapData bitmapData2;
		Gdiplus::Rect r(0, 0, value.first->GetWidth(), value.first->GetHeight());
		value.first->LockBits(&r, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, value.first->GetPixelFormat(), &bitmapData2);
		int num = GetPixelFormatSize(bitmapData2.PixelFormat) / 8;
		int array2Length = bitmapData2.Stride * bitmapData2.Height;
		unsigned char* array2 = (unsigned char*)bitmapData2.Scan0;
		value.second = CalculateOpaqueBounds(array2, bitmapData2.Width, bitmapData2.Height, num, bitmapData2.Stride);
		for (int i = 0; i < array2Length; i += num)
		{
			Gdiplus::Color color(array2[i + 2], array2[i + 1], array2[i]);
			float num2 = ToLinear(color.GetR());
			float num3 = ToLinear(color.GetG());
			float num4 = ToLinear(color.GetB());
			float num5 = num2;
			float num6 = num3;
			float num7 = num4;
			std::tuple<float, float, float, float> valueTuple = std::tuple<float, float, float, float>(0.0f, -0.333333343f, 2.0f / 3.0f, -1.0f);
			std::tuple<float, float, float, float> valueTuple2 = (num6 >= num7) ? std::tuple<float, float, float, float>(num6, num7, std::get<0>(valueTuple), std::get<1>(valueTuple)) : std::tuple<float, float, float, float>(num7, num6, std::get<3>(valueTuple), std::get<2>(valueTuple));
			std::tuple<float, float, float, float> valueTuple3 = (num5 >= std::get<0>(valueTuple2)) ? std::tuple<float, float, float, float>(num5, std::get<1>(valueTuple2), std::get<2>(valueTuple2), std::get<0>(valueTuple2)) : std::tuple<float, float, float, float>(std::get<0>(valueTuple2), std::get<1>(valueTuple2), std::get<3>(valueTuple2), num5);
			float num8 = std::get<0>(valueTuple3) - min(std::get<3>(valueTuple3), std::get<1>(valueTuple3));
			num3 = 1E-10f;
			float num9 = num8;
			float num10 = num3;
			num3 = abs(std::get<2>(valueTuple3) + (std::get<3>(valueTuple3) - std::get<1>(valueTuple3)) / (6.0f * num9 + num10));
			num2 = num9 / (std::get<0>(valueTuple3) + num10);
			float item = std::get<0>(valueTuple3);
			float num11 = num3;
			float num12 = num2;
			float num13 = item;
			float num14 = GetHue(teamColor->LowerBounds) / 360;
			float num15 = GetHue(teamColor->UpperBounds) / 360;
			if (num11 >= num14 && num15 >= num11)
			{
				num11 = num11 / (num15 - num14) * (num15 + teamColor->Fudge - (num14 - teamColor->Fudge));
				num11 += teamColor->HSVShift.X;
				num12 += teamColor->HSVShift.Y;
				num13 += teamColor->HSVShift.Z;
				std::tuple<float, float, float, float> valueTuple4 = std::tuple<float, float, float, float>(1.0f, 2.0f / 3.0f, 0.333333343f, 3.0f);
				std::tuple<float, float, float> valueTuple5 = std::tuple<float, float, float>(abs(frac(num11 + std::get<0>(valueTuple4)) * 6.0f - std::get<3>(valueTuple4)), abs(frac(num11 + std::get<1>(valueTuple4)) * 6.0f - std::get<3>(valueTuple4)), abs(frac(num11 + std::get<2>(valueTuple4)) * 6.0f - std::get<3>(valueTuple4)));
				num5 = num13 * lerp(std::get<0>(valueTuple4), saturate(std::get<0>(valueTuple5) - std::get<0>(valueTuple4)), num12);
				num6 = num13 * lerp(std::get<0>(valueTuple4), saturate(std::get<1>(valueTuple5) - std::get<0>(valueTuple4)), num12);
				num7 = num13 * lerp(std::get<0>(valueTuple4), saturate(std::get<2>(valueTuple5) - std::get<0>(valueTuple4)), num12);
				std::tuple<float, float, float> valueTuple6 = std::tuple<float, float, float>(min(1, max(0, num5 - teamColor->InputLevels.X) / (teamColor->InputLevels.Z - teamColor->InputLevels.X)), min(1, max(0, num6 - teamColor->InputLevels.X) / (teamColor->InputLevels.Z - teamColor->InputLevels.X)), min(1, max(0, num7 - teamColor->InputLevels.X) / (teamColor->InputLevels.Z - teamColor->InputLevels.X)));
				std::get<0>(valueTuple6) = (float)pow(std::get<0>(valueTuple6), teamColor->InputLevels.Y);
				std::get<1>(valueTuple6) = (float)pow(std::get<1>(valueTuple6), teamColor->InputLevels.Y);
				std::get<2>(valueTuple6) = (float)pow(std::get<2>(valueTuple6), teamColor->InputLevels.Y);
				num5 = lerp(teamColor->OutputLevels.X, teamColor->OutputLevels.Y, std::get<0>(valueTuple6));
				num6 = lerp(teamColor->OutputLevels.X, teamColor->OutputLevels.Y, std::get<1>(valueTuple6));
				num7 = lerp(teamColor->OutputLevels.X, teamColor->OutputLevels.Y, std::get<2>(valueTuple6));
			}
			std::tuple<float, float, float> valueTuple7 = std::tuple<float, float, float>(min(1, max(0, num5 - teamColor->OverallInputLevels.X) / (teamColor->OverallInputLevels.Z - teamColor->OverallInputLevels.X)), min(1, max(0, num6 - teamColor->OverallInputLevels.X) / (teamColor->OverallInputLevels.Z - teamColor->OverallInputLevels.X)), min(1, max(0, num7 - teamColor->OverallInputLevels.X) / (teamColor->OverallInputLevels.Z - teamColor->OverallInputLevels.X)));
			std::get<0>(valueTuple7) = (float)pow(std::get<0>(valueTuple7), teamColor->OverallInputLevels.Y);
			std::get<1>(valueTuple7) = (float)pow(std::get<1>(valueTuple7), teamColor->OverallInputLevels.Y);
			std::get<2>(valueTuple7) = (float)pow(std::get<2>(valueTuple7), teamColor->OverallInputLevels.Y);
			num5 = lerp(teamColor->OverallOutputLevels.X, teamColor->OverallOutputLevels.Y, std::get<0>(valueTuple7));
			num6 = lerp(teamColor->OverallOutputLevels.X, teamColor->OverallOutputLevels.Y, std::get<1>(valueTuple7));
			num7 = lerp(teamColor->OverallOutputLevels.X, teamColor->OverallOutputLevels.Y, std::get<2>(valueTuple7));
			array2[i + 2] = (byte)(ToSRGB(num5) * 255);
			array2[i + 1] = (byte)(ToSRGB(num6) * 255);
			array2[i] = (byte)(ToSRGB(num7) * 255);
		}
		value.first->UnlockBits(&bitmapData2);
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
