#include "global.h"
#include "textman.h"
#include "megfilemanager.h"
#include "wwfile.h"
#include <locale>
#include <codecvt>
struct StringTableRecord
{
	unsigned int crc;
	unsigned int valuelen;
	unsigned int keylen;
};
TextManager::TextManager(MegFileManager* manager, const char* file)
{
	FileClass *f = manager->Open(file);
	if (f)
	{
		int count;
		f->Read(&count, sizeof(count));
		StringTableRecord* r = new StringTableRecord[count];
		f->Read(r, count * sizeof(StringTableRecord));
		wchar_t** values = new wchar_t* [count];
		for (int i = 0; i < count; i++)
		{
			values[i] = new wchar_t[r[i].valuelen + 1];
			values[i][r[i].valuelen] = 0;
			f->Read(values[i], r[i].valuelen * 2);
		}
		for (int i = 0; i < count; i++)
		{
			char *key = new char[r[i].keylen + 1];
			key[r[i].keylen] = 0;
			f->Read(key, r[i].keylen);
			char* value = new char[r[i].valuelen + 1];
			wcstombs(value, values[i], r[i].valuelen + 1);
			gameText[key] = value;
			delete[] value;
			delete[] values[i];
		}
		delete[] values;
	}
}

std::string TextManager::Get(const char* id)
{
	std::string str;
	if (!TryGetValue(gameText, id, str))
	{
		return id;
	}
	return str;
}

TextManager* TheTextManager;
void InitText()
{
	TheTextManager = new TextManager(TheMegFileManager, "DATA\\TEXT\\MASTERTEXTFILE_EN-US.LOC");
}

void ShutdownText()
{
	delete TheTextManager;
}
