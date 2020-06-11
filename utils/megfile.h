#pragma once
#include <unordered_map>
#include <string>
#include <stdio.h>
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
	std::unordered_map<std::string, SubFileData> subfiles;
	std::unordered_map<int, std::string> filenames;
	MegHeader header;
	FILE* handle;
public:
	MegFile(const char *path);
	~MegFile();
	int GetFileCount();
	const char *GetFileName(int index);
	int GetFileSize(const char* file);
	void GetFileData(const char* file, unsigned char* data);
};