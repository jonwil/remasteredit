#pragma once
class MegFileManager;
class TextManager
{
private:
	std::map<std::string, std::string> gameText;
public:
	TextManager(MegFileManager* manager, const char* file);
	std::string Get(const char* id);
};

extern TextManager* TheTextManager;
void InitText();
void ShutdownText();
