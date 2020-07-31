#pragma once
class Widget;
class ImageList
{
private:
	HIMAGELIST Handle;
public:
	ImageList(Gdiplus::Size size);
	~ImageList();
	int AddImage(Gdiplus::Bitmap* bitmap);
	HIMAGELIST GetHandle() { return Handle; }
};
class ListView
{
private:
	HWND Handle;
	struct ListViewItem
	{
		int Group;
		const char* Name;
		int ImageIndex;
		LPARAM Object;
	};
	std::map<int, ListViewItem> Items;
	int SelectedIndex;
	LPARAM SelectedObject;
	int GroupID;
public:
	Widget* Widget;
	ListView(int x, int y, int width, int height, HWND parent, HINSTANCE instance);
	~ListView();
	void BeginUpdate();
	void EndUpdate();
	void EnsureVisible(int index);
	void OnItemChanged(LPNMLISTVIEW item);
	int AddGroup(const char* name);
	int AddItem(const char* name, int group, int imageindex, LPARAM object);
	void SelectItemByObject(LPARAM object);
	void SetImageList(ImageList* list);
	void SetItemCount(int count);
	int GetSelectedIndex() { return SelectedIndex; }
	LPARAM GetSelectedObject() { return SelectedObject; }
	HWND GetHandle() { return Handle; }
};
