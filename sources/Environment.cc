///
///
#include "Precompiled.h"
//
#include <stdlib.h> // size_t, malloc, free
#include <new> // bad_alloc, bad_array_new_length
#include <Pathcch.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#include <vector>
//
#include <algorithm>
///
#include "Environment.hpp"

#if defined(_MSC_VER) &&_MSC_VER <=1800 && __cplusplus<201103L
#define noexcept
#endif

template <class T> struct Mallocator {
	typedef T value_type;
	Mallocator() noexcept{} // default ctor not required
		template <class U> Mallocator(const Mallocator<U>&) noexcept{}
		template <class U> bool operator==(
		const Mallocator<U>&) const noexcept{ return true; }
		template <class U> bool operator!=(
		const Mallocator<U>&) const noexcept{ return false; }

		T * allocate(const size_t n) const
	{
		if (n == 0) { return nullptr; }
		if (n > static_cast<size_t>(-1) / sizeof(T)) {
			throw std::bad_array_new_length();
		}
		void * const pv = malloc(n * sizeof(T));
		if (!pv) { throw std::bad_alloc(); }
		return static_cast<T *>(pv);
	}
	void deallocate(T * const p, size_t) const noexcept{
		free(p);
	}
};

template <class TBase> inline TBase *ZeroMemoryAlloc(size_t n) {
  TBase *Ptr = reinterpret_cast<TBase *>(malloc(sizeof(TBase) * n));
  if (Ptr) {
    memset(Ptr, 0, sizeof(TBase) * n);
  }
  return Ptr;
}

inline void MemoryFree(void *ptr) { free(ptr); }

/*
 Build PATH Environment Variable
*/
bool EnvironmentPathBuilder(std::wstring &paths) {
  WCHAR maxBuf[MAX_PATH] = {0};
  // Because Windows support Install other disk partition.
  // so your should find WINDIR from Registry or Environment
  auto ret = GetEnvironmentVariableW(L"WINDIR", maxBuf, MAX_PATH);
  if (ret == 0)
    return false;
  paths.assign(maxBuf);
  paths.append(LR"(\System32;)");
  paths.append(maxBuf);
  paths.push_back(';');
  paths.append(maxBuf);
  paths.append(LR"(\System32\Wbem;)");
  paths.append(maxBuf);
  paths.append(LR"(\System32\WindowsPowerShell\v1.0;)");
  return true;
}

bool PathsCombine(const std::vector<std::wstring> &pathv, std::wstring &paths) {
  for (auto &p : pathv) {
    paths.append(p);
    paths.push_back(L';');
  }
  return true;
}

BOOL WINAPI IsAdministrator() {
  BOOL b = FALSE;
  SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
  PSID AdministratorsGroup;
  b = AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                               DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0,
                               &AdministratorsGroup);
  if (b) {
    if (!CheckTokenMembership(NULL, AdministratorsGroup, &b)) {
      b = FALSE;
    }
    FreeSid(AdministratorsGroup);
  }

  return (b);
}

AppLoaderEnvironmentStrings::AppLoaderEnvironmentStrings() : envbuf(nullptr) {
  ///
}

AppLoaderEnvironmentStrings::~AppLoaderEnvironmentStrings() {
  if (envbuf) {
    /// to do Remove
    // HeapFree(GetProcessHeap(), 0, envbuf);
    MemoryFree(envbuf);
  }
}

bool AppLoaderEnvironmentStrings::InitializeEnvironment() {
  auto env = GetEnvironmentStringsW();
  if (env == nullptr)
    return false;
  wchar_t *pl = nullptr;
  for (pl = env; *pl; pl++) {
    envlist.push_back(pl);
    pl += wcslen(pl);
  }
  FreeEnvironmentStringsW(env);
  return true;
}
const wchar_t *AppLoaderEnvironmentStrings::EnvironmentBuilder() {
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
    Offbuf += c.size() + 1;
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

bool AppLoaderEnvironmentStrings::Delete(const wchar_t *key) {
  std::wstring key_(key);
  key_.push_back('=');
  for (auto iter = envlist.begin(); iter != envlist.end(); iter++) {
    if ((*iter).compare(0, key_.size(), key_.data()) == 0) {
      envlist.erase(iter);
      updateCount++;
      return true;
    }
  }
  return false;
}
bool AppLoaderEnvironmentStrings::Append(const wchar_t *key,
                                         const std::wstring &va) {
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

bool AppLoaderEnvironmentStrings::Insert(const wchar_t *key,
                                         const std::wstring &va) {
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

bool AppLoaderEnvironmentStrings::Replace(const wchar_t *key,
                                          const std::wstring &va) {
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

// DoEnvironmentSubstW
// ExpandEnvironmentStringsW
// PathProcessCommand

inline bool PathIsRelativeInternal(const wchar_t *file) {
  if (*file && file[1] == ':')
    return false;
  return true;
}

HRESULT PathResolveAppRoot(wchar_t *parent, size_t length,
                           const wchar_t *path) {
  HRESULT hr = S_OK;
  if (PathIsRelativeInternal(path)) {
    std::vector<wchar_t> bufvector(length);
    wchar_t *buffer = bufvector.data();
    GetCurrentDirectoryW(bufvector.size(), buffer);
    hr = PathCchCombineEx(parent, length, buffer, path,
                          PATHCCH_ALLOW_LONG_PATHS);
  } else {
    // PathCanonicalizeW(parent, path);
    hr = PathCchCombineEx(parent, length, path, NULL, PATHCCH_ALLOW_LONG_PATHS);
  }
  hr = PathCchRemoveFileSpec(parent, length);
  return hr;
}

AppLoaderEnvironment::AppLoaderEnvironment() {
  ////
}

bool AppLoaderEnvironment::Initialize(const wchar_t *alfile) {
  wchar_t buf[PATHCCH_MAX_CCH];
  auto hr = PathResolveAppRoot(buf, PATHCCH_MAX_CCH, alfile);
  if (hr == S_OK) {
    builtEnv.insert(std::pair<std::wstring, std::wstring>(L"APPROOT", buf));
  }
  //// TO add more environment
  return true;
}
enum KEnvStateMachine : int {
  kClearReset = 0,
  kEscapeAllow = 1,
  kMarkAllow = 2,
  kBlockBegin = 3,
  kBlockEnd = 4
};

bool AppLoaderEnvironment::DoEnvironmentSubstW(std::wstring &str) {
  if (str.empty())
    return false;
  std::wstring ns, ks;
  auto p = str.c_str();
  auto n = str.size();
  int pre = 0;
  size_t i = 0;
  KEnvStateMachine state = kClearReset;
  for (; i < n; i++) {
    switch (p[i]) {
    case '`': {
      switch (state) {
      case kClearReset:
        state = kEscapeAllow;
        break;
      case kEscapeAllow:
        ns.push_back('`');
        state = kClearReset;
        break;
      case kMarkAllow:
        state = kEscapeAllow;
        ns.push_back('$');
        break;
      case kBlockBegin:
        continue;
      default:
        ns.push_back('`');
        continue;
      }
    } break;
    case '$': {
      switch (state) {
      case kClearReset:
        state = kMarkAllow;
        break;
      case kEscapeAllow:
        ns.push_back('$');
        state = kClearReset;
        break;
      case kMarkAllow:
        ns.push_back('$');
        state = kClearReset;
        break;
      case kBlockBegin:
      case kBlockEnd:
      default:
        ns.push_back('$');
        continue;
      }
    } break;
    case '{': {
      switch (state) {
      case kClearReset:
      case kEscapeAllow:
        ns.push_back('{');
        state = kClearReset;
        break;
      case kMarkAllow: {
        state = kBlockBegin;
        pre = i;
      } break;
      case kBlockBegin:
        ns.push_back('{');
        break;
      default:
        continue;
      }
    } break;
    case '}': {
      switch (state) {
      case kClearReset:
      case kEscapeAllow:
        ns.push_back('}');
        state = kClearReset;
        break;
      case kMarkAllow:
        state = kClearReset;
        ns.push_back('$');
        ns.push_back('}');
        break;
      case kBlockBegin: {
        ks.assign(&p[pre + 1], i - pre - 1);
        std::wstring v;
        if (QueryEnvironmentVariableU(ks, v))
          ns.append(v);
        state = kClearReset;
      } break;
      default:
        continue;
      }
    } break;
    default: {
      switch (state) {
      case kClearReset:
        ns.push_back(p[i]);
        break;
      case kEscapeAllow:
        ns.push_back('`');
        ns.push_back(p[i]);
        state = kClearReset;
        break;
      case kMarkAllow:
        ns.push_back('$');
        ns.push_back(p[i]);
        state = kClearReset;
        break;
      case kBlockBegin:
      default:
        continue;
      }
    } break;
    }
  }
  str.assign(ns);
  return true;
}

bool AppLoaderEnvironment::QueryEnvironmentVariableU(const std::wstring &key,
                                                     std::wstring &value) {
  std::wstring ukey; /// convet to upper, and find it
  for (auto c : key) {
	  ukey.push_back(::toupper(c));
  }
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

bool EnvironmentResolvePathEx(AppLoaderEnvironmentStrings &aes,
                              const std::vector<std::wstring> &paths,
                              bool isclean) {
  if (!aes.InitializeEnvironment()) {
    return false;
  }
  if (paths.size() > 0) {
    std::wstring path_;
    for (auto &p : paths) {
      path_.append(p);
      path_.push_back(';');
    }
    if (isclean) {
      std::wstring paths;
      if (!EnvironmentPathBuilder(paths))
        return 1;
      paths.append(path_);
      aes.Replace(L"PATH", paths);
    } else {
      aes.Insert(L"PATH", path_);
    }
  }
  return true;
}