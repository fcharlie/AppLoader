///
///
#include "Precompiled.h"
//
#include <Shlwapi.h>
#include <Shlobj.h>
#include <vector>
#include <Pathcch.h>
//
#include <algorithm>
///
#include "Environment.hpp"
//DoEnvironmentSubstW
//ExpandEnvironmentStringsW
//PathProcessCommand

inline bool PathIsRelativeInternal(const wchar_t *file)
{
	if (*file&&file[1] == ':')
		return false;
	return true;
}

HRESULT PathResolveAppRoot(wchar_t *parent, size_t length, const wchar_t *path)
{
	HRESULT hr = S_OK;
	if (PathIsRelativeInternal(path)) {
		std::vector<wchar_t> bufvector(length);
		wchar_t *buffer = bufvector.data();
		GetCurrentDirectoryW(bufvector.size(),buffer);
		hr=PathCchCombineEx(parent, length, buffer, path, PATHCCH_ALLOW_LONG_PATHS);
	} else {
		//PathCanonicalizeW(parent, path);
		hr=PathCchCombineEx(parent, length, path, NULL, PATHCCH_ALLOW_LONG_PATHS);
	}
	hr=PathCchRemoveFileSpec(parent, length);
	return hr;
}

AppLoaderEnvironment::AppLoaderEnvironment()
{
	////
}

bool AppLoaderEnvironment::Initialize(const wchar_t *alfile)
{
	wchar_t buf[PATHCCH_MAX_CCH];
	auto hr=PathResolveAppRoot(buf, PATHCCH_MAX_CCH, alfile);
	if (hr == S_OK) {
		builtEnv.insert(std::pair<std::wstring, std::wstring>(L"APPROOT", buf));
	}
	////
	return true;
}

bool AppLoaderEnvironment::DoEnvironmentSubst(std::wstring &str)
{
	std::wstring tmp;
	std::wstring key;
	//raw Pointer
	auto begin = str.data();
	auto end = begin + str.size();
	for (; begin < end; begin++) {
		if (*begin == '$') {
			while (++begin < end) {

			}
		}
		tmp.push_back(*begin);
	}
	return true;
}

bool AppLoaderEnvironment::ExpendEnvironment(const std::wstring &key, std::wstring &value)
{
	std::wstring ukey; /// convet to upper, and find it
	std::transform(key.begin(), key.end(), ukey.begin(), ::toupper); 
	auto iter = builtEnv.find(ukey);
	if (iter != builtEnv.end()) {
		value.assign(iter->second);
		return true;
	}
	wchar_t va[PATHCCH_MAX_CCH];
	auto l = GetEnvironmentVariableW(key.c_str(), va, PATHCCH_MAX_CCH);
	if (l == 0) {
		return false;
	}
	value.assign(va, (size_t)l);
	return true;
}