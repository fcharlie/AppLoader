///
///
////
///
#include <string>
#include <windows.h>
#include <algorithm>
#include <tuple>
////
#include <AppLoaderFile.h>

enum FileMemViewState {
	kFileMemViewSuccess = 0,
	kFileMemViewInvalidHandle,
	kFileMemViewInvalidSize,
	kFileMemAllocateOutOfMemory,
	kFileMemReadFailed
};

static uint16_t ByteSwap(uint16_t i)
{
	uint16_t j;
	j = (i << 8);
	j += (i >> 8);
	return j;
}

static inline void ByteSwapShortBuffer(WCHAR *buffer, int len)
{
	int i;
	uint16_t *sb = reinterpret_cast<uint16_t*>(buffer);
	for (i = 0; i<len; i++)
		sb[i] = ByteSwap(sb[i]);
}

struct BaseLine {
	wchar_t *Ptr;
	uint32_t n;
};

class AppLoaderFileMemView {
public:
	AppLoaderFileMemView() :Ptr(nullptr)
	{
		///
	}
	~AppLoaderFileMemView()
	{
		/// release
	}
	int FileMemViewStringMount(const wchar_t *file, size_t limitsize = 0x8000)
	{
		_ASSERT(file);
		HANDLE hFile = CreateFileW(file,
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
		size_t size = static_cast<size_t>(largeFile.QuadPart);
		uint8_t *baseAddress = (uint8_t*)malloc(size+2);
		if (baseAddress == nullptr) return kFileMemAllocateOutOfMemory;
		baseAddress[size] = 0;
		baseAddress[size + 1] = 0;
		DWORD dwRead = 0;
		if (!ReadFile(hFile, baseAddress, (DWORD)(size), &dwRead, nullptr)) {
			CloseHandle(hFile);
			free(baseAddress);
			return kFileMemReadFailed;
		}
		if (baseAddress[0] == 0xFF && baseAddress[1] == 0xFE) { /// UTF16LE
			Ptr = (wchar_t*)(baseAddress + 2);
			raw = baseAddress;
			end_=Ptr+dwRead/2-1;
		} else if (baseAddress[0] == 0xFE && baseAddress[1] == 0xFF) { /// UTF16 BE
			this->Ptr = (wchar_t*)(baseAddress + 2);
			raw = baseAddress;
			end_ = Ptr+dwRead / 2 - 1;
			ByteSwapShortBuffer(this->Ptr, (dwRead - 2) / sizeof(wchar_t));
		} else {
			char *offsetPtr=(char*)baseAddress;
			auto l = dwRead;
			if (baseAddress[0]==0xEF&&baseAddress[1]==0xBB&&baseAddress[2]==0xBF) {
				offsetPtr += 3;
				l = l - 3;
			}
			auto len = MultiByteToWideChar(CP_UTF8, 0, const_cast<const char*>(offsetPtr), l, NULL, 0);
			wchar_t *NewPtr = (wchar_t *)malloc(len*sizeof(wchar_t) + 1);
			if (NewPtr==nullptr) {
				CloseHandle(hFile);
				free(baseAddress);
				return kFileMemAllocateOutOfMemory;
			}
			NewPtr[len] = 0;
			MultiByteToWideChar(CP_UTF8, 0, const_cast<const char*>(offsetPtr), l, NewPtr, len);
			Ptr = NewPtr;
			end_ = Ptr + len;
			raw = NewPtr;
			/// TO Convert
		}
		CloseHandle(hFile);
		return kFileMemViewSuccess;
	}
private:
	void *raw;
	wchar_t *Ptr;
	wchar_t *end_;
};

AppLoaderFile::AppLoaderFile(const wchar_t *file) : metafile(file) {
	view = new AppLoaderFileMemView();
}
AppLoaderFile::~AppLoaderFile()
{
	if (view) {
		delete view;
	}
}

bool AppLoaderFile::Parse()
{
	if (view->FileMemViewStringMount(this->metafile)!=kFileMemViewSuccess)
		return false;
	return true;
}

BOOL WINAPI AppLoaderFilterIcon(LPCWSTR file, LPWSTR iconfile, DWORD counts) {
  WCHAR exe[4096];
  auto l =
      GetPrivateProfileStringW(L"Base", L"Executable", NULL, exe, 4096, file);
  if (l == 0)
    return FALSE;
  l = GetPrivateProfileStringW(L"Base", L"Icon", exe, iconfile, counts, file);
  return l > 0;
}
