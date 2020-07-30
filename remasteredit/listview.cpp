#include "global.h"
#include "listview.h"
#include "tools.h"
ImageList::ImageList(Gdiplus::Size size)
{
	Handle = ImageList_Create(size.Width, size.Height, ILC_MASK | ILC_COLOR24, 4, 4);
}

ImageList::~ImageList()
{
	ImageList_Destroy(Handle);
}

HBITMAP CreateMask(Gdiplus::Bitmap* bitmap)
{
	Gdiplus::Size size(bitmap->GetWidth(), bitmap->GetHeight());
	int width = bitmap->GetWidth();
	int height = bitmap->GetHeight();
	int stride = width / 8;
	if ((width % 8) != 0)
	{
		stride++;
	}
	if ((stride % 2) != 0)
	{
		stride++;
	}
	BYTE *bits = new BYTE[stride * height];
	memset(bits, 0, stride * height);
	Gdiplus::Rect r(0, 0, width, height);
	Gdiplus::BitmapData bitmapData;
	bitmap->LockBits(&r, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);
	unsigned char* data = static_cast<unsigned char*>(bitmapData.Scan0);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int color = *reinterpret_cast<int*>(&data[y * bitmapData.Stride + x * 4]);
			if (color >> 24 == 0)
			{
				int index = stride * y + x / 8;
				bits[index] |= (BYTE)(0x80 >> (x % 8));
			}
		}
	}
	bitmap->UnlockBits(&bitmapData);
	HBITMAP h = CreateBitmap(size.Width, size.Height, 1, 1, bits);
	delete[] bits;
	return h;
}

PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
{
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD    cClrBits;

	// Retrieve the bitmap color format, width, and height.  
	GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp);

	// Convert the color format to a count of bits.  
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else cClrBits = 32;

	// Allocate memory for the BITMAPINFO structure. (This structure  
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
	// data structures.)  

	if (cClrBits < 24)
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * (1 << cClrBits));

	// There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

	else
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER));

	// Initialize the fields in the BITMAPINFO structure.  

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24)
		pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

	// If the bitmap is not compressed, set the BI_RGB flag.  
	pbmi->bmiHeader.biCompression = BI_RGB;

	// Compute the number of bytes in the array of color  
	// indices and store the result in biSizeImage.  
	// The width must be DWORD aligned unless the bitmap is RLE 
	// compressed. 
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
		* pbmi->bmiHeader.biHeight;
	// Set biClrImportant to 0, indicating that all of the  
	// device colors are important.  
	pbmi->bmiHeader.biClrImportant = 0;
	return pbmi;
}

void CreateBMPFile(LPTSTR pszFile, HBITMAP hBMP)
{
	HANDLE hf;                 // file handle  
	BITMAPFILEHEADER hdr;       // bitmap file-header  
	PBITMAPINFOHEADER pbih;     // bitmap info-header  
	LPBYTE lpBits;              // memory pointer  
	DWORD dwTotal;              // total count of bytes  
	DWORD cb;                   // incremental count of bytes  
	BYTE* hp;                   // byte pointer  
	DWORD dwTmp;
	PBITMAPINFO pbi;
	HDC hDC;

	hDC = CreateCompatibleDC(GetWindowDC(GetDesktopWindow()));
	SelectObject(hDC, hBMP);

	pbi = CreateBitmapInfoStruct(hBMP);

	pbih = (PBITMAPINFOHEADER)pbi;
	lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	// Retrieve the color table (RGBQUAD array) and the bits  
	// (array of palette indices) from the DIB.  
	GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi,
		DIB_RGB_COLORS);

	// Create the .BMP file.  
	hf = CreateFile(pszFile,
		GENERIC_READ | GENERIC_WRITE,
		(DWORD)0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);

	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
	// Compute the size of the entire file.  
	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	// Compute the offset to the array of color indices.  
	hdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD);

	// Copy the BITMAPFILEHEADER into the .BMP file.  
	WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
		(LPDWORD)&dwTmp, NULL);

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
	WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
		+ pbih->biClrUsed * sizeof(RGBQUAD),
		(LPDWORD)&dwTmp, (NULL));

	// Copy the array of color indices into the .BMP file.  
	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL);

	// Close the .BMP file.  
	CloseHandle(hf);

	// Free memory.  
	GlobalFree((HGLOBAL)lpBits);
}

HBITMAP CreateColor(Gdiplus::Bitmap* bitmap, HBITMAP mask)
{
	Gdiplus::Size size(bitmap->GetWidth(), bitmap->GetHeight());
	HBITMAP bm;
	bitmap->GetHBITMAP(Gdiplus::ARGB(Gdiplus::Color::LightGray), &bm);
	HDC dc = GetDC(nullptr);
	HDC source = CreateCompatibleDC(dc);
	HDC target = CreateCompatibleDC(dc);
	ReleaseDC(nullptr, dc);
	HGDIOBJ ps = SelectObject(source, mask);
	HGDIOBJ pt = SelectObject(target, bm);
	SetBkColor(target, 0x00ffffff);
	SetTextColor(target, 0);
	BitBlt(target, 0, 0, size.Width, size.Height, source, 0, 0, 0x220326);
	SelectObject(source, ps);
	SelectObject(target, pt);
	DeleteDC(source);
	DeleteDC(target);
	return bm;
}

int i = 0;
int ImageList::AddImage(Gdiplus::Bitmap* bitmap)
{
	HBITMAP mask = CreateMask(bitmap);
	HBITMAP bm = CreateColor(bitmap, mask);
	int index = ImageList_Add(Handle, mask, bm);
	DeleteObject(mask);
	DeleteObject(bm);
	return index;
}

ListView::ListView(int x, int y, int width, int height, HWND parent, HINSTANCE instance) : SelectedIndex(-1), SelectedObject(0), Widget(nullptr), GroupID(0)
{
	Handle = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "templateTypeListView", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_TABSTOP | LVS_AUTOARRANGE | LVS_SHOWSELALWAYS | LVS_SINGLESEL, x, y, width, height, parent, nullptr, instance, nullptr);
	ListView_EnableGroupView(Handle, TRUE);
}

ListView::~ListView()
{
}

void ListView::BeginUpdate()
{
	SendMessage(Handle, WM_SETREDRAW, FALSE, 0);
}

void ListView::EndUpdate()
{
	SendMessage(Handle, WM_SETREDRAW, TRUE, 0);
	InvalidateRect(Handle, nullptr, FALSE);
}

void ListView::EnsureVisible(int index)
{
	ListView_EnsureVisible(Handle, index, false);
}

void ListView::OnItemChanged(LPNMLISTVIEW item)
{
	if (item->uChanged == LVIF_STATE)
	{
		SelectedIndex = ListView_GetNextItem(Handle, -1, LVNI_SELECTED);
		if (SelectedIndex != -1)
		{
			LVITEM lvi;
			memset(&lvi, 0, sizeof(LVITEM));
			lvi.iItem = SelectedIndex;
			lvi.mask = LVIF_PARAM;
			ListView_GetItem(Handle, &lvi);
			SelectedObject = lvi.lParam;
		}
		else
		{
			SelectedObject = 0;
		}
		if (Widget)
		{
			Widget->OnSelChanged();
		}
	}
}

int ListView::AddGroup(const char* name)
{
	LVGROUP group;
	memset(&group, 0, sizeof(LVGROUP));
	group.cbSize = sizeof(LVGROUP);
	group.mask = LVGF_HEADER | LVGF_GROUPID | LVGF_ALIGN;
	group.uAlign = LVGA_HEADER_LEFT;
	size_t size = mbstowcs(nullptr, name, 0);
	wchar_t* str = new wchar_t[size + 1];
	mbstowcs(str, name, size + 1);
	group.pszHeader = str;
	group.cchHeader = wcslen(str);
	group.iGroupId = GroupID;
	GroupID++;
	ListView_InsertGroup(Handle, -1, &group);
	delete[] str;
	return group.iGroupId;
}

int ListView::AddItem(const char* name, int group, int imageindex, LPARAM object)
{
	LVITEM lvitem;
	memset(&lvitem, 0, sizeof(LVITEM));
	lvitem.mask = LVIF_GROUPID | LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvitem.iItem = 0xFFFF;
	lvitem.iGroupId = group;
	lvitem.pszText = const_cast<LPSTR>(name);
	lvitem.lParam = object;
	lvitem.iImage = imageindex;
	int index = ListView_InsertItem(Handle, &lvitem);
	ListViewItem item;
	item.Group = group;
	item.ImageIndex = imageindex;
	item.Name = name;
	item.Object = object;
	Items[index] = item;
	return index;
}

void ListView::SelectItemByObject(LPARAM object)
{
	LVFINDINFO findInfo;
	memset(&findInfo, 0, sizeof(findInfo));
	findInfo.flags = LVFI_PARAM;
	findInfo.lParam = static_cast<LPARAM>(object);
	int itemIndex = ListView_FindItem(Handle, -1, &findInfo);
	ListView_SetItemState(Handle, itemIndex, LVIS_SELECTED, LVIS_SELECTED);
}

void ListView::SetImageList(ImageList* list)
{
	ListView_SetImageList(Handle, list->GetHandle(), LVSIL_NORMAL);
}
