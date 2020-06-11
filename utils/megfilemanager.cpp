#include "megfilemanager.h"
#include "megfile.h"
#include <cctype>
bool MegFileManager::Load(const char* megFilePath)
{
	MegFile* m = new MegFile(megFilePath);
	if (!m->Is_Open())
	{
		delete m;
		return false;
	}
	for (int i = 0; i < m->GetFileCount(); i++)
	{
		filenames.insert(m->GetFileName(i));
	}
	megfiles.push_back(m);
	return true;
}

bool MegFileManager::Exists(const char* path)
{
	std::string str = loosepath;
	if (str[str.length() - 1] != '\\')
	{
		str += '\\';
	}
	str += path;
	Stream s(path);
	if (s.Is_Available())
	{
		return true;
	}
	else
	{
		std::string p = path;
		std::for_each(p.begin(), p.end(), [](char& c) {c = (char)std::toupper(c); });
		if (filenames.find(p) == filenames.end())
		{
			return true;
		}
	}
	return false;
}

Stream* MegFileManager::Open(const char* path)
{
	std::string str = loosepath;
	if (str[str.length() - 1] != '\\')
	{
		str += '\\';
	}
	str += path;
	Stream *s = new Stream(path);
	if (s->Is_Available())
	{
		return s;
	}
	else
	{
		delete s;
		std::string p = path;
		std::for_each(p.begin(), p.end(), [](char& c) {c = (char)std::toupper(c); });
		for (auto i : megfiles)
		{
			s = i->OpenFile(p.c_str());
			if (s->Is_Available())
			{
				return s;
			}
			i->CloseFile(s);
		}
	}
	return nullptr;
}

void MegFileManager::Close(Stream *s)
{
	megfiles[0]->CloseFile(s);
}

MegFileManager::~MegFileManager()
{
	for (auto i : megfiles)
	{
		delete i;
	}
	megfiles.clear();
}
