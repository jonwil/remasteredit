#pragma once
#include <unordered_map>
#include <string>
#include "wwfile.h"
struct MegHeader
{
	unsigned int Flags;
	unsigned int ID;
	unsigned int DataOffset;
	unsigned int FilenameCount;
	unsigned int FileCount;
	unsigned int FileTableSize;
};
struct SubFileData
{
	unsigned short Flags;
	unsigned int CRCValue;
	int SubfileIndex;
	unsigned int SubfileSize;
	unsigned int SubfileImageDataOffset;
	unsigned short SubfileNameIndex;
};

class MegFile
{
private:
	std::string fname;
	std::unordered_map<std::string, SubFileData> subfiles;
	std::unordered_map<int, std::string> filenames;
	MegHeader header;
	FileClass* stream;
public:
	MegFile(const char *path);
	~MegFile();
	int GetFileCount();
	const char *GetFileName(int index);
	FileClass* OpenFile(const char *filename);
	void CloseFile(FileClass* s);
	bool Is_Open()
	{
		return stream != nullptr;
	}
};
