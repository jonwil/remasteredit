#include "global.h"
#include "listview.h"
#include "tools.h"
ImageList::ImageList(Gdiplus::Size size)
{
	Handle = ImageList_Create(size.Width, size.Height, ILC_MASK | ILC_COLOR32 | ILC_ORIGINALSIZE, 4, 4);
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

int ImageList::AddImage(Gdiplus::Bitmap* bitmap)
{
	HBITMAP mask = CreateMask(bitmap);
	HBITMAP bm = CreateColor(bitmap, mask);
	int index = ImageList_Add(Handle, bm, mask);
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

void ListView::SetItemCount(int count)
{
	ListView_SetItemCount(Handle, count);
}
