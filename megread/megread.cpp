#include "megfile.h"
int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("meg file reader\n");
		printf("usage megpath [filepath]\n");
		printf("megpath is the path to a .meg file\n");
		printf("filepath is the path of a file within that .meg file\n");
		printf("if you don't specify a file path, it will print a list of all the files in that .meg file\n");
		printf("otherwise it will extract the specified file\n");
		return 1;
	}
	MegFile m(argv[1]);
	if (argc < 3)
	{
		for (int i = 0; i < m.GetFileCount(); i++)
		{
			printf("%s\n", m.GetFileName(i));
		}
	}
	Stream* s = m.OpenFile(argv[2]);
	int size = s->Size();
	if (size != 0)
	{
		unsigned char* c = new unsigned char[size];
		s->Read(c,size);
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
