///
///
////
///
#include <string>
#include <windows.h>
#include <AppLoaderFile.h>

int AppLoaderFile(const std::wstring &file)
{
	HANDLE hFile = CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == NULL)
		return 1;
	return 0;
}

int WINAPI AppLoaderExtraIcon(LPCWSTR file,LPWSTR iconfile,size_t counts)
{
	GetPrivateProfileStringW(L"Base", L"Icon", L"Z", iconfile, counts, file);
	if (wcslen(iconfile) == 1) {

	}
	return 0;
}