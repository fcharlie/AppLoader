///
///
#ifndef APPLOADER_FILE_H
#define APPLOADER_FILE_H
#define WIN32_LEAN_AND_MEAN
#include <stdint.h>
#include <string>
#include <vector>
#include <chrono>
#include <windows.h>

enum AppLoaderNodeItem {
	kNodeString,
	kNodeInteger,
	kNodeBoolean,
	kNodeArray
};

struct ParseEvent {
	std::wstring setion;
	int(*impl)(const wchar_t *key, void *value, int flags);
};

class AppLoaderFileMemView;
class AppLoaderFile {
public:
  AppLoaderFile(const wchar_t *file);
  ~AppLoaderFile();
  bool Parse(); 
private:
  const wchar_t *metafile;
  AppLoaderFileMemView *view;
};

BOOL WINAPI AppLoaderFilterIcon(LPCWSTR file, LPWSTR iconfile, DWORD counts); 



#endif
