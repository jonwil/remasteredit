#include <string.h>
#include "megfilemanager.h"
#include "megfile.h"
#include "RAWFILE.H"
MegFileManager* TheMegFileManager = nullptr;
void LoadMegs(const char *path)
{
	TheMegFileManager = new MegFileManager(path);
	std::string p1 = path;
	p1 += "\\";
	p1 += "DATA";
	p1 += "\\";
	std::string p2 = p1;
	p2 += "CONFIG.MEG";
	TheMegFileManager->Load(p2.c_str());
	p2 = p1;
	p2 += "TEXTURES_COMMON_SRGB.MEG";
	TheMegFileManager->Load(p2.c_str());
	p2 = p1;
	p2 += "TEXTURES_RA_SRGB.MEG";
	TheMegFileManager->Load(p2.c_str());
	p2 = p1;
	p2 += "TEXTURES_SRGB.MEG";
	TheMegFileManager->Load(p2.c_str());
	p2 = p1;
	p2 += "TEXTURES_TD_SRGB.MEG";
	TheMegFileManager->Load(p2.c_str());
}

void CloseMegs()
{
	delete TheMegFileManager;
}

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
	RawFileClass s(path);
	if (s.Is_Available())
	{
		return true;
	}
	else
	{
		char *p = _strdup(path);
		_strupr(p);
		if (filenames.find(p) != filenames.end())
		{
			return true;
		}
	}
	return false;
}

FileClass* MegFileManager::Open(const char* path)
{
	std::string str = loosepath;
	if (str[str.length() - 1] != '\\')
	{
		str += '\\';
	}
	str += path;
	FileClass* s = new RawFileClass(path);
	if (s->Is_Available())
	{
		return s;
	}
	else
	{
		delete s;
		char* p = _strdup(path);
		_strupr(p);
		for (auto i : megfiles)
		{
			s = i->OpenFile(p);
			if (s && s->Is_Available())
			{
				return s;
			}
			i->CloseFile(s);
		}
	}
	return nullptr;
}

void MegFileManager::Close(FileClass *s)
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
