#include "megfile.h"
#include "RAWFILE.H"
MegFile::MegFile(const char* path)
{
	fname = path;
	stream = new RawFileClass(path);
	if (!stream->Is_Available())
	{
		delete stream;
		return;
	}
	stream->Open();
	stream->Read(&header,sizeof(header));
	char* filenamedata = new char[header.FileTableSize];
	stream->Read(filenamedata, header.FileTableSize);
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
		stream->Read(&data.Flags, sizeof(data.Flags));
		stream->Read(&data.CRCValue, sizeof(data.CRCValue));
		stream->Read(&data.SubfileIndex, sizeof(data.SubfileIndex));
		stream->Read(&data.SubfileSize, sizeof(data.SubfileSize));
		stream->Read(&data.SubfileImageDataOffset, sizeof(data.SubfileImageDataOffset));
		stream->Read(&data.SubfileNameIndex, sizeof(data.SubfileNameIndex));
		filenames[data.SubfileIndex] = names[data.SubfileNameIndex];
		subfiles[names[data.SubfileNameIndex]] = data;
	}
	delete[] filenamedata;
}

MegFile::~MegFile()
{
	if (stream)
	{
		delete stream;
	}
}

int MegFile::GetFileCount()
{
	return header.FileCount;
}

const char* MegFile::GetFileName(int index)
{
	if (filenames.find(index) != filenames.end())
	{
		return filenames[index].c_str();
	}
	return nullptr;
}

FileClass* MegFile::OpenFile(const char* filename)
{
	char* p = _strdup(filename);
	_strupr(p);
	if (subfiles.find(p) != subfiles.end())
	{
		RawFileClass* s = new RawFileClass(fname.c_str());
		if (s->Is_Available())
		{
			SubFileData data = subfiles[p];
			s->Bias(data.SubfileImageDataOffset, data.SubfileSize);
			s->Open();
			return s;
		}
	}
	return nullptr;
}

void MegFile::CloseFile(FileClass* s)
{
	if (s)
	{
		delete s;
	}
}

