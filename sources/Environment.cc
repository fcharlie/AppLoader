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

template<class TBase>
inline TBase* ZeroMemoryAlloc(size_t n)
{
	TBase *Ptr = reinterpret_cast<TBase*>(malloc(sizeof(TBase)*n));
	if (Ptr) {
		memset(Ptr, 0, sizeof(TBase)*n);
	}
	return Ptr;
}

inline void MemoryFree(void* ptr)
{
	free(ptr);
}


/*
 Build PATH Environment Variable
*/
bool EnvironmentPathBuilder(std::wstring &paths)
{
	WCHAR maxBuf[MAX_PATH] = { 0 };
	auto ret=GetEnvironmentVariableW(L"WINDIR", maxBuf, MAX_PATH);
	if (ret == 0)
		return false;
	paths.assign(maxBuf);
	paths.append(L"\\System32;");
	paths.append(maxBuf);
	paths.push_back(';');
	paths.append(maxBuf);
	paths.append(L"\\System32\Wbem;");
	paths.append(maxBuf);
	paths.append(L"\\System32\\WindowsPowerShell\\v1.0;");
	return true;
}


AppLoaderEnvironmentStrings::AppLoaderEnvironmentStrings() :envbuf(nullptr)
{
	///
}

AppLoaderEnvironmentStrings::~AppLoaderEnvironmentStrings()
{
	if (envbuf) {
		/// to do Remove
		//HeapFree(GetProcessHeap(), 0, envbuf);
		MemoryFree(envbuf);
	}
}

bool AppLoaderEnvironmentStrings::InitializeEnvironment()
{
	auto env = GetEnvironmentStringsW();
	if (env == nullptr) return false;
	wchar_t *pl=nullptr;
	for (pl = env; *pl; pl++) {
		envlist.push_back(pl);
		pl += wcslen(pl);
	}
	FreeEnvironmentStringsW(env);
	return true;
}
const wchar_t *AppLoaderEnvironmentStrings::EnvironmentBuilder()
{
	if (envbuf && updateCount == lastBuilderCount)
		return envbuf;
	size_t length = 0;
	for (auto &c : envlist) {
		length += c.size() + 1;
	}
	length++;
	wchar_t *NewBuf = ZeroMemoryAlloc<wchar_t>(length);
	wchar_t *Offbuf = NewBuf;
	for (auto &c : envlist) {
		wmemcpy(Offbuf, c.data(), c.size());
		Offbuf += c.size()+1;
	}
	*Offbuf = 0;
	lastBuilderCount = updateCount;
	if (envbuf) {
		MemoryFree(envbuf);
		envbuf = NewBuf;
	} else {
		envbuf = NewBuf;
	}
	return envbuf;
}

bool AppLoaderEnvironmentStrings::Delete(const wchar_t *key)
{
	std::wstring key_(key);
	key_.push_back('=');
	for (auto iter = envlist.begin();iter != envlist.end(); iter++) {
		if ((*iter).compare(0, key_.size(), key_.data()) == 0) {
			envlist.erase(iter);
			updateCount++;
			return true;
		}
	}
	return false;
}
bool AppLoaderEnvironmentStrings::Append(const wchar_t *key, const std::wstring &va)
{
	std::wstring key_(key);
	key_.push_back('=');
	for (auto iter = envlist.begin(); iter != envlist.end(); iter++) {
		if (iter->compare(0, key_.size(), key_.data()) == 0) {
			iter->push_back(L';');
			iter->append(va);
			updateCount++;
			return true;
		}
	}
	return true;
}

bool AppLoaderEnvironmentStrings::Insert(const wchar_t *key, const std::wstring &va)
{
	std::wstring key_(key);
	key_.push_back('=');
	for (auto iter = envlist.begin(); iter != envlist.end(); iter++) {
		if (iter->compare(0, key_.size(), key_.data()) == 0) {
			iter->insert(key_.size(), va.data());
			updateCount++;
			return true;
		}
	}
	return true;
}

bool AppLoaderEnvironmentStrings::Replace(const wchar_t *key, const std::wstring &va)
{
	std::wstring key_(key);
	key_.push_back('=');
	for (auto iter = envlist.begin(); iter != envlist.end(); iter++) {
		if (iter->compare(0, key_.size(), key_.data()) == 0) {
			iter->replace(key_.size(), iter->size() - key_.size(), va.data());
			updateCount++;
			return true;
		}
	}
	return true;
}

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
			if (++begin >= end) {
				return false;
			} 
			if (*begin != '{') {
				tmp.push_back('$');
				tmp.push_back(*begin);
				continue;
			}
			while (begin&&*begin != '}') {

			}
		}
		tmp.push_back(*begin);
	}
	return true;
}

bool AppLoaderEnvironment::QueryEnvironmentVariableU(const std::wstring &key, std::wstring &value)
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