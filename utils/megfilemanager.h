#pragma once
#include <vector>
#include <string>
#include <unordered_set>
class FileClass;
class MegFile;
class MegFileManager
{
private:
	std::vector<MegFile *> megfiles;
	std::string loosepath;
	std::unordered_set<std::string> filenames;
public:
	MegFileManager(const char* looseFilePath)
	{
		loosepath = looseFilePath;
	}
	bool Load(const char* megafilePath);
	bool Exists(const char* path);
	FileClass*Open(const char* path);
	~MegFileManager();
};

extern MegFileManager* TheMegFileManager;
void LoadMegs(const char* path);
void CloseMegs();
