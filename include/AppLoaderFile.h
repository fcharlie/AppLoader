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

#define APPLOADER_FILE_SIZE_LIMIT 8192

enum kAppLoaderFileItemType {
	kAppLoaderFileItemString,
	kAppLoaderFileItemBoolean,
	kAppLoaderFileItemInteger,
	kAppLoaderFileItemUInteger,
	kAppLoaderFileItemFloat,
	kAppLoaderFileItemDatetime
};

class AppLoaderFileItem {
public:
	AppLoaderFileItem(bool b)
	{
		item.Boolean = b;
		kind = kAppLoaderFileItemBoolean;
	}
	AppLoaderFileItem(uint64_t ui)
	{
		item.UInteger = ui;
		kind = kAppLoaderFileItemUInteger;
	}
	AppLoaderFileItem(int64_t i)
	{
		item.Integer = i;
		kind = kAppLoaderFileItemInteger;
	}
	AppLoaderFileItem(double f)
	{
		item.Float = f;
		kind = kAppLoaderFileItemFloat;
	}
	AppLoaderFileItem(const wchar_t *s, size_t n)
	{
		item.String = new std::wstring(s, n);
		kind = kAppLoaderFileItemString;
	}
	AppLoaderFileItem(const std::wstring &s)
	{
		item.String = new std::wstring(s);
		kind = kAppLoaderFileItemString;
	}
	~AppLoaderFileItem()
	{
		if (kind == kAppLoaderFileItemString)
			delete this->item.String;
	}
private:
	int kind;
	union internal_type {
		std::wstring *String;
		uint64_t UInteger;
		int64_t Integer;
		bool Boolean;
		double Float;
		time_t time;
	}item;
};

class AppLoaderFile {
public:
  AppLoaderFile(const wchar_t *file);
  bool Parse(); /// Full Parse module,
  
private:
  const wchar_t *metafile;
};

BOOL WINAPI AppLoaderExtraIcon(LPCWSTR file, LPWSTR iconfile, DWORD counts); // Use WinAPI 
bool FasterResolveIcon(const wchar_t *file,wchar_t *receive, uint32_t *counts);



#endif
