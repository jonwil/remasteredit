#pragma once
class StatusBar
{
public:
	int Count;
	std::string* strings;
	StatusBar(int count);
	void Create(HINSTANCE instance, HWND parent);
	void OnSize();
	void SetText(int pos, const char* str);
	HWND hWndStatusBar;
};
