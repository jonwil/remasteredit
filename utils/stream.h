#pragma once
#include <string>
#include <cstdio>
#include <algorithm>
class Stream {
private:
	int BiasStart;
	int BiasLength;
	FILE* Handle;
	std::string Filename;
public:
	Stream()
	{
		BiasLength = -1;
		Handle = nullptr;
		BiasStart = 0;
		Filename = "";
	}
	Stream(const char* filename) : Filename(filename)
	{
		BiasLength = -1;
		Handle = nullptr;
		BiasStart = 0;
	}
	void Reset()
	{
		Close();
		Filename = "";
	}
	~Stream()
	{
		Reset();
	}
	const char* Set_Name(const char* name)
	{
		Reset();
		if (name)
		{
			Bias(0, -1);
			Filename = name;
		}
		return nullptr;
	}
	bool Is_Available()
	{
		if (!Filename.length())
		{
			return false;
		}
		if (Is_Open())
		{
			return true;
		}
		FILE* f = fopen(Filename.c_str(), "rb");
		if (f)
		{
			fclose(f);
			return true;
		}
		return false;
	}
	bool Is_Open() const
	{
		if (Handle)
		{
			return true;
		}
		return false;
	}
	bool Open(const char* name)
	{
		Set_Name(name);
		return Open();
	}
	bool Open()
	{
		Close();
		if (!Filename.length())
		{
			return false;
		}
		Handle = fopen(Filename.c_str(), "rb");
		if (Handle)
		{
			if ((BiasStart) && (BiasLength != -1))
			{
				Seek(0, 0);
			}
			return true;
		}
		return false;
	}
	int Read(void* buffer, int bytesToRead)
	{
		int bytesRead = 0;
		if (!Is_Open())
		{
			if (Open())
			{
				bytesRead = Read(buffer, bytesToRead);
				Close();
			}
		}
		else
		{
			if (BiasLength != -1)
			{
				int maxBytesToRead = BiasLength - Tell();
				if (bytesToRead > maxBytesToRead)
				{
					bytesToRead = maxBytesToRead;
				}
			}
			fread(buffer, 1, bytesToRead, Handle);
		}
		return bytesRead;
	}
	template<typename T> int Read(T& buf)
	{
		return Read(&buf, sizeof(T));
	}
	int Seek(int pos, int dir)
	{
		if (BiasLength != -1)
		{
			switch (dir)
			{
			case 1:
				pos += Raw_Seek(0, 1) - BiasStart;
				break;
			case 2:
				pos += BiasLength;
				break;
			}
			int result = Raw_Seek(BiasStart + std::clamp(pos, 0, BiasLength), 0);
			if (result != -1)
			{
				result -= BiasStart;
			}
			return result;
		}
		else
		{
			return Raw_Seek(pos, dir);
		}
	}
	int Raw_Seek(int pos, int dir)
	{
		if (!Is_Open())
		{
			return 0;
		}
		if (fseek(Handle, pos, dir))
		{
			return 0;
		}
		int seek = ftell(Handle);
		return seek;
	}
	int Size()
	{
		if (BiasLength != -1)
		{
			return BiasLength;
		}
		else
		{
			int size = -1;
			if (!Is_Open())
			{
				if (Open())
				{
					size = Size();
					Close();
				}
			}
			else
			{
				int tell = ftell(Handle);
				fseek(Handle, 0, SEEK_END);
				size = ftell(Handle);
				fseek(Handle, tell, SEEK_SET);
			}
			return size;
		}
	}
	void Close()
	{
		if (Is_Open())
		{
			fclose(Handle);
			Handle = nullptr;
		}
	}
	void Bias(int start, int length)
	{
		if (start == 0)
		{
			BiasStart = 0;
			BiasLength = -1;
		}
		else
		{
			BiasLength = Size();
			BiasStart += start;
			if (length != -1)
			{
				if (length < BiasLength)
				{
					BiasLength = length;
				}
				if (BiasLength < 0)
				{
					BiasLength = 0;
				}
			}
		}
	}
	int Tell()
	{
		return Seek(0, 1);
	}
};
