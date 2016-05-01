//
///
//
//
#ifndef STREAM_HPP
#define STREAM_HPP
#include <string>
#include <Windows.h>

class FileStream {
public:
	FileStream();
	bool Parse(const std::wstring &file);
private:
	HANDLE hFile;
};

#endif
