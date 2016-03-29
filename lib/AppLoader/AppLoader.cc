///
///
////
///
#include <AppLoaderFile.h>
#include <string>
#include <windows.h>

AppLoaderFile::AppLoaderFile(const wchar_t *file) : metafile(file) {}

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