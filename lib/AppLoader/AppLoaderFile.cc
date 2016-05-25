///
///
////
///
#include <algorithm>
#include <fstream>
#include <string>
#include <tuple>
#include <windows.h>
////
#include <AppLoaderFile.h>

enum FileMemViewState {
  kFileMemViewSuccess = 0,
  kFileMemViewInvalidHandle,
  kFileMemViewInvalidSize,
  kFileMemAllocateOutOfMemory,
  kFileMemReadFailed
};

static uint16_t ByteSwap(uint16_t i) {
  uint16_t j;
  j = (i << 8);
  j += (i >> 8);
  return j;
}

static inline void ByteSwapShortBuffer(WCHAR *buffer, int len) {
  int i;
  uint16_t *sb = reinterpret_cast<uint16_t *>(buffer);
  for (i = 0; i < len; i++)
    sb[i] = ByteSwap(sb[i]);
}

struct FlowLine {
  wchar_t *Ptr;
  uint32_t n;
};

class AppLoaderFileMemView {
public:
  AppLoaderFileMemView() : Ptr(nullptr) {
    ///
  }
  ~AppLoaderFileMemView() {
    /// release
	if(raw){
		free(raw);
	}
  }
  int FileMemViewStringMount(const wchar_t *file, size_t limitsize = 0x8000) {
    _ASSERT(file);
    HANDLE hFile = CreateFileW(file, GENERIC_READ, FILE_SHARE_READ, NULL,
                               OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
      return kFileMemViewInvalidHandle;
    LARGE_INTEGER largeFile;
    if (!GetFileSizeEx(hFile, &largeFile))
      return kFileMemViewInvalidSize;
    if (largeFile.QuadPart < 4 || largeFile.QuadPart > limitsize)
      return kFileMemViewInvalidSize;
    size_t size = static_cast<size_t>(largeFile.QuadPart);
    uint8_t *baseAddress = (uint8_t *)malloc(size + 2);
    if (baseAddress == nullptr)
      return kFileMemAllocateOutOfMemory;
    baseAddress[size] = 0;
    baseAddress[size + 1] = 0;
    DWORD dwRead = 0;
    if (!ReadFile(hFile, baseAddress, (DWORD)(size), &dwRead, nullptr)) {
      CloseHandle(hFile);
      free(baseAddress);
      return kFileMemReadFailed;
    }
    if (baseAddress[0] == 0xFF && baseAddress[1] == 0xFE) { /// UTF16LE
      Ptr = (wchar_t *)(baseAddress + 2);
      raw = baseAddress;
      end_ = Ptr + dwRead / 2 - 1;
    } else if (baseAddress[0] == 0xFE && baseAddress[1] == 0xFF) { /// UTF16 BE
      this->Ptr = (wchar_t *)(baseAddress + 2);
      raw = baseAddress;
      end_ = Ptr + dwRead / 2 - 1;
      ByteSwapShortBuffer(this->Ptr, (dwRead - 2) / sizeof(wchar_t));
    } else {
      char *offsetPtr = (char *)baseAddress;
      auto l = dwRead;
      if (baseAddress[0] == 0xEF && baseAddress[1] == 0xBB &&
          baseAddress[2] == 0xBF) {
        offsetPtr += 3;
        l = l - 3;
      }
      auto len = MultiByteToWideChar(
          CP_UTF8, 0, const_cast<const char *>(offsetPtr), l, NULL, 0);
      wchar_t *NewPtr = (wchar_t *)malloc(len * sizeof(wchar_t) + 1);
      if (NewPtr == nullptr) {
        CloseHandle(hFile);
        free(baseAddress);
        return kFileMemAllocateOutOfMemory;
      }
      NewPtr[len] = 0;
      MultiByteToWideChar(CP_UTF8, 0, const_cast<const char *>(offsetPtr), l,
                          NewPtr, len);
      Ptr = NewPtr;
      end_ = Ptr + len;
      raw = NewPtr;
      /// TO Convert
    }
    CloseHandle(hFile);
    return kFileMemViewSuccess;
  }
  using iterator = std::wstring::iterator;
  const wchar_t *begin() const { return this->Ptr; }
  const wchar_t *end() const { return this->end_; }
  bool getline(FlowLine &line) {
    auto _begin = Ptr;
    if (Ptr == end_)
      return false;
    for (; Ptr != end_ && *Ptr != '\n'; Ptr++) {
    }
    line.Ptr = _begin;
    line.n = Ptr - _begin;
    if (*Ptr == '\n' && Ptr != end_)
      Ptr++;
    return true;
  }

private:
  void *raw;
  wchar_t *Ptr;
  wchar_t *end_;
};

AppLoaderFile::AppLoaderFile(const wchar_t *file) : metafile(file) {
  view = new AppLoaderFileMemView();
}
AppLoaderFile::~AppLoaderFile() {
  if (view) {
    delete view;
  }
}

inline bool StringCaseCompareWhitespace(const std::wstring &cstr,
                                        wchar_t *begin, wchar_t *end) {
  if ((size_t)(end - begin) < cstr.size())
    return false;
  wchar_t l, f;
  auto iter = begin;
  auto iter2 = cstr.data();
  auto count = cstr.size();
  do {
    l = toupper(*iter);
    f = toupper(*iter2);
    iter++;
    iter2++;
  } while ((--count) && iter2 && (l == f));
  if (l - f != 0)
    return false;
  if (count == 0) {
    while (iter != end) {
      switch (*iter) {
      case ' ':
      case '\r':
        break;
      default:
        return false;
      }
    }
  }
  return true;
}

bool AppLoaderFile::Parse() {
  if (view->FileMemViewStringMount(this->metafile) != kFileMemViewSuccess)
    return false;
  std::wstring section(L"(default).");
  std::wstring key;
  FlowLine line;
  auto ParseString = [&](wchar_t *iter, wchar_t *end) {
    if (++iter < end) {
      auto string_start = iter;
      for (; iter < end && *iter != '"'; iter++)
        ;
      if (*iter == '"') {
        Objects_.insert(std::pair<std::wstring, Object>(
            key, std::move(Object(string_start, iter - string_start))));
      }
    }
  };
  auto ParseBoolean = [&](wchar_t *iter, wchar_t *end) {
    if (StringCaseCompareWhitespace(L"TRUE", iter, end)) {
      Objects_.insert(std::pair<std::wstring, Object>(key, Object(true)));
    } else if (StringCaseCompareWhitespace(L"FALSE", iter, end)) {
      Objects_.insert(std::pair<std::wstring, Object>(key, Object(false)));
    } else {
      /// throw error !
    }
  };
  auto ParseNumber = [&](wchar_t *iter, wchar_t *end) {
    int64_t num = 0;
    std::wstring wcs(iter, end);
    wchar_t *c;
    num = wcstoll(wcs.c_str(), &c, 10);
    Objects_.insert(std::pair<std::wstring, Object>(key, Object(num)));
  };
  auto ParseArray = [&](wchar_t *iter, wchar_t *end) {
    Object obj;
    iter++;
    bool seq = false;
    while (*iter != ']') {
      if (iter != end && (*iter == ' ' || *iter == '\t' || *iter == '\r')) {
        iter++;
        continue;
      }
      if (iter == end) {
        if (!view->getline(line))
          return false;
        iter = line.Ptr;
        end = line.Ptr + line.n;
        continue;
      }
      if (*iter++ == '"') {
        auto l = iter;
        while (iter != end && *iter != '"') {
          iter++;
        }
        if (*iter == '"') {
          obj.add(Object(l, iter - l));
          iter++;
        }
        continue;
      }
    }
    Objects_.insert(std::pair<std::wstring, Object>(key, std::move(obj)));
    return true;
  };
  while (view->getline(line)) {
    if (line.n == 0)
      continue;
    if (line.Ptr[0] == '#')
      continue;
    ///// Parse section
    auto iter = line.Ptr;
    auto end = line.Ptr + line.n;
    if (*iter == '[') {
      for (; iter < end; iter++) {
        if (*iter == ']') {
          section.assign(line.Ptr + 1, iter - line.Ptr - 1);
          section.push_back('.');
          break;
        }
      }
      continue;
    }
    key.assign(section);
    for (; iter < end && *iter != '='; iter++) {
      key.push_back(*iter);
    }
    if (*iter == '=') {
      if (++iter != end) {
        if (*iter == '"') {
          ParseString(iter, end);
        } else if (*iter == 'T' || *iter == 't' || *iter == 'F' ||
                   *iter == 'f') {
          ParseBoolean(iter, end);
        } else if (*iter >= '0' && *iter <= '9') {
          ParseNumber(iter, end);
        } else if (*iter == '[') {
          ParseArray(iter, end);
        } else {
          continue;
        }
      }
    }
    //////////// Parse
  }
  auto i = Objects_.size();
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
