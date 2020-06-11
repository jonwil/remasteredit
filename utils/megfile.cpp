#include "megfile.h"
MegFile::MegFile(const char* path)
{
	handle = fopen(path, "rb");
	fread(&header, sizeof(header), 1, handle);
	char* filenamedata = new char[header.FileTableSize];
	fread(filenamedata, 1, header.FileTableSize, handle);
	std::vector<std::string> names;
	char* filenameptr = filenamedata;
	for (unsigned int i = 0; i < header.FilenameCount; i++)
	{
		unsigned short size = *(unsigned short*)filenameptr;
		filenameptr += 2;
		char* filename = new char[size + 1];
		memcpy(filename, filenameptr, size);
		filename[size] = 0;
		filenameptr += size;
		names.push_back(filename);
	}
	for (unsigned int i = 0; i < header.FileCount; i++)
	{
		SubFileData data;
		fread(&data.Flags, sizeof(data.Flags), 1, handle);
		fread(&data.CRCValue, sizeof(data.CRCValue), 1, handle);
		fread(&data.SubfileIndex, sizeof(data.SubfileIndex), 1, handle);
		fread(&data.SubfileSize, sizeof(data.SubfileSize), 1, handle);
		fread(&data.SubfileImageDataOffset, sizeof(data.SubfileImageDataOffset), 1, handle);
		fread(&data.SubfileNameIndex, sizeof(data.SubfileNameIndex), 1, handle);
		filenames[data.SubfileIndex] = names[data.SubfileNameIndex];
		subfiles[names[data.SubfileNameIndex]] = data;
	}
	delete[] filenamedata;
}

MegFile::~MegFile()
{
	fclose(handle);
}

int MegFile::GetFileCount()
{
	return header.FileCount;
}

const char* MegFile::GetFileName(int index)
{
	return filenames[index].c_str();
}

int MegFile::GetFileSize(const char* file)
{
	return subfiles[file].SubfileSize;
}

void MegFile::GetFileData(const char* file, unsigned char* data)
{
	fseek(handle, subfiles[file].SubfileImageDataOffset, SEEK_SET);
	fread(data, 1, subfiles[file].SubfileSize, handle);
}
