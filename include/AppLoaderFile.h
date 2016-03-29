///
///
#ifndef APPLOADER_FILE_H
#define APPLOADER_FILE_H
#define WIN32_LEAN_AND_MEAN
#include <stdint.h>
#include <windows.h>


class AppLoaderFile {
public:
  AppLoaderFile(const wchar_t *file);
  bool Parse(); /// Full Parse module,
  bool FasterParse(wchar_t *receive, uint32_t *counts);

private:
  const wchar_t *metafile;
};

BOOL WINAPI AppLoaderExtraIcon(LPCWSTR file, LPWSTR iconfile, DWORD counts);

#endif
