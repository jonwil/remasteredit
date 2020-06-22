#include "megfilemanager.h"
#include "megfile.h"
int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		printf("meg file get\n");
		printf("usage gamepath [filepath]\n");
		printf("gamepath is the path to the game install\n");
		printf("filepath is the path of a file from the game data\n");
		printf("this tool will use the same file search logic as the Petroglyph editor does to locate and extract the specified file");
		return 1;
	}
	LoadMegs(argv[1]);
	FileClass* s = TheMegFileManager->Open(argv[2]);
	int size = s->Size();
	if (size != 0)
	{
		unsigned char* c = new unsigned char[size];
		s->Read(c, size);
		const char* name = strrchr(argv[2], '\\');
		if (!name)
		{
			name = argv[2];
		}
		else
		{
			name++;
		}
		FILE* f = fopen(name, "wb");
		fwrite(c, 1, size, f);
		fclose(f);
		delete[] c;
	}
	CloseMegs();
	return 0;
}
