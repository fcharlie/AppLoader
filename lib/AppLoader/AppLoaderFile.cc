///
///
////
///
#include <AppLoaderFile.h>
#include <string>
#include <windows.h>
#include <algorithm>
#include <AppLoaderFile.h>

enum FileMemViewState {
	kFileMemViewSuccess = 0,
	kFileMemViewInvalidHandle,
	kFileMemViewInvalidSize,
	kFileMemAllocateOutOfMemory
};



class AppLoaderFileMemView {
public:
	AppLoaderFileMemView() :hFile(nullptr)
	{
		///
	}
	~AppLoaderFileMemView()
	{
		if (hFile&&hFile != INVALID_HANDLE_VALUE) {
			CloseHandle(hFile);
		}
		/// release
	}
	int FileMemViewStringMount(const wchar_t *file, size_t limitsize = 0x8000)
	{
		_ASSERT(file);
		hFile = CreateFileW(file,
							GENERIC_READ,
							FILE_SHARE_READ,
							NULL,
							OPEN_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return kFileMemViewInvalidHandle;
		LARGE_INTEGER largeFile;
		if (!GetFileSizeEx(hFile, &largeFile))
			return kFileMemViewInvalidSize;
		if (largeFile.QuadPart<4 || largeFile.QuadPart>limitsize)
			return kFileMemViewInvalidSize;
		return kFileMemViewSuccess;
	}
private:
	int ConvertFile()
	{
		return true;
	}
	HANDLE hFile;
	wchar_t *Ptr;
	size_t length;
};

//template<class CharT = wchar_t>
//class AppLoaderChunk {
//public:
//	typedef std::iterator iterator;
//	enum NewLineEnum {
//		kLF,
//		kCRLF,
//		kCR
//	};
//	struct Line {
//		CharT *data;
//		size_t length;
//		int newline;
//	};
//#define ZERO_LINE Line{0,0}
//	AppLoaderChunk() {}
//	wchar_t *begin()
//	{
//		return data;
//	}
//	wchar_t *end()
//	{
//		return data + count;
//	}
//	Line Next()
//	{
//		return ZERO_LINE;
//	}
//private:
//	T *data;
//	T *cur;
//	size_t count;
//};

AppLoaderFile::AppLoaderFile(const wchar_t *file) : metafile(file) {
	//
}


bool AppLoaderFile::Parse()
{
	///
	return true;
}

bool FasterResolveIcon(const wchar_t *file, wchar_t *receive, uint32_t *counts)
{
	if (file == nullptr) {
		return false;
	}
	WCHAR buffer[8192] = { 0 };
	DWORD bytes = 0;
	HANDLE hFile = CreateFileW(file, GENERIC_READ, FILE_SHARE_READ, NULL,
							   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == NULL)
		return false;
	if (!ReadFile(hFile, buffer, 8192 * 2, &bytes, NULL)) {
		return false;
	}
	auto n = bytes / 2;
	wchar_t *p = buffer;
	buffer[n] = L'\0';
	for (auto i = 1; i < n; i++) {
		if (buffer[i] == L'['&&buffer[i - 1] == '\n') {
			if (wcscmp(&buffer[i], L"[Base]\r\n") == 0) {
				p = &buffer[i] + 8;
				break;
			}
			if (wcscmp(&buffer[i], L"[Base]\n") == 0) {
				p = &buffer[i] + 7;
				break;
			}
		}
	}
	if (p == buffer)
		return false; /// not found section

	return true;
}





int AppLoaderFile(const std::wstring &file) {
  HANDLE hFile = CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == NULL)
    return 1;
  return 0;
}

BOOL WINAPI AppLoaderExtraIcon(LPCWSTR file, LPWSTR iconfile, DWORD counts) {
  WCHAR exe[4096];
  auto l =
      GetPrivateProfileStringW(L"Base", L"Executable", NULL, exe, 4096, file);
  if (l == 0)
    return FALSE;
  l = GetPrivateProfileStringW(L"Base", L"Icon", exe, iconfile, counts, file);
  return l > 0;
}