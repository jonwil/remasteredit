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
	MegFileManager m(argv[1]);
	std::string p1 = argv[1];
	p1 += "\\";
	p1 += "DATA";
	p1 += "\\";
	std::string p2 = p1;
	p2 += "CONFIG.MEG";
	m.Load(p2.c_str());
	p2 = p1;
	p2 += "TEXTURES_COMMON_SRGB.MEG";
	m.Load(p2.c_str());
	p2 = p1;
	p2 += "TEXTURES_RA_SRGB.MEG";
	m.Load(p2.c_str());
	p2 = p1;
	p2 += "TEXTURES_SRGB.MEG";
	m.Load(p2.c_str());
	p2 = p1;
	p2 += "TEXTURES_TD_SRGB.MEG";
	m.Load(p2.c_str());
	Stream* s = m.Open(argv[2]);
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
	return 0;
}
