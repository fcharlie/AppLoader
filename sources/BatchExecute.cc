///
///
#include "Precompiled.h"
////
#include <stdint.h>
#include <Pathcch.h>
#include <Shellapi.h>
///
#include "Environment.hpp"
#include "Executable.hpp"
#include "Execute.hpp"
//


class CharactersConvert {
public:
  CharactersConvert(const wchar_t *wstr) : length_(0) {
    if (wstr == nullptr)
      return;
    int iTextLen =
        WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
    data_ = new char[iTextLen + 1];
    if (data_ == nullptr)
      return;
    data_[iTextLen] = 0;
    WideCharToMultiByte(CP_ACP, 0, wstr, -1, data_, iTextLen, NULL, NULL);
    length_ = iTextLen;
  }
  ~CharactersConvert() {
    if (data_) {
      delete[] data_;
      //// to free data
    }
  }
  const char *Ptr() { return data_; }
  uint32_t Length() { return length_; }

private:
  char *data_;
  uint32_t length_;
};

/*
* cmd.exe Not support UTF16 ,so we convert to CP_ACP
*
*/
class BatchFileBuilder {
public:
  BatchFileBuilder() : hFile(nullptr) {
    ///
  }
  ~BatchFileBuilder() {
    if (hFile != nullptr && hFile != INVALID_HANDLE_VALUE) {
      CloseHandle(hFile);
    }
	if (initialized) {
		DeleteFileW(path_.data());
	}
  }
  bool Initialize() {
    WCHAR buf[MAX_PATH];
    auto dwRetVal = GetTempPathW(MAX_PATH, buf);
    if (dwRetVal > MAX_PATH || dwRetVal == 0) {
      return false;
    }
    path_.assign(buf);
	swprintf_s(buf, L"%sAppLoaderBatch.%d.bat", path_.data(), GetCurrentThreadId());
    path_.assign(buf);
    hFile = CreateFileW(path_.data(),          // file name
                               GENERIC_WRITE,         // open for write
                               0,                     // do not share
                               NULL,                  // default security
                               CREATE_ALWAYS,         // overwrite existing
                               FILE_ATTRIBUTE_NORMAL, // normal file
                               NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
      return false;
    }
	initialized = true;
    return true;
  }
  DWORD Write(const char *text, DWORD size) {
    DWORD dwBytes = 0;
    if (WriteFile(hFile, text, size, &dwBytes, NULL)) {
      return dwBytes;
    }
    return 0;
  }
  const wchar_t *Path() { return path_.c_str(); }
  void Flush() {
	  CloseHandle(hFile);
	  hFile = nullptr;
  }

private:
  HANDLE hFile;
  std::wstring path_;
  bool initialized = false;
};

int BatchAttachExecute(const ExecutableFile &exe) {
  BatchFileBuilder batchFileBuilder;
  if (!batchFileBuilder.Initialize()) {
    /// to do something
    return 1;
  }
  const char *header = "@echo off \n";
  batchFileBuilder.Write(header, sizeof(header) - 1);
  std::wstring inits = L"call ";
  inits.append(exe.InitializeScript());
  inits.append(L" \n");
  CharactersConvert civ(inits.data());
  batchFileBuilder.Write(civ.Ptr(), civ.Length());
  std::wstring wenv;
  if (exe.IsClearEnvironment()) {
    if (!EnvironmentPathBuilder(wenv)) {
      return 2;
    }
  } else {
    wenv.assign(L"%PATH%;");
  }
  auto &path = exe.Path();
  if (path.size()) {
    std::wstring ws = L"set PATH=";
    std::wstring s;
    for (auto &p : path) {
      s.append(p);
      s.push_back(L';');
    }
    ws.append(s);
    ws.append(wenv);
    ws.append(L" \n");
    CharactersConvert cv(ws.data());
    batchFileBuilder.Write(cv.Ptr(), cv.Length());
  }
  std::wstring start = L"start ";
  if (exe.StartupDir().size()) {
    start.append(L" /D ");
    start.append(exe.StartupDir());
    start.push_back(L' ');
  }
  if (exe.Executable().find(L' ') < exe.Executable().size()) {
    start.push_back('"');
    start.append(exe.Executable());
    start.append(L"\" ");
  } else {
    start.append(exe.Executable());
    start.push_back(' ');
  }
  for (auto &a : exe.Args()) {
    if (a.find(L" ") < a.size()) {
      start.push_back('"');
      start.append(a);
      start.append(L"\" ");
    } else {
      start.append(a);
      start.push_back(' ');
    }
  }
  start.append(L" \n");
  CharactersConvert cs(start.data());
  batchFileBuilder.Write(cs.Ptr(), cs.Length());
  batchFileBuilder.Flush(); // flush buffer to disk
  WCHAR maxCmd[PATHCCH_MAX_CCH] = L"/c \"";
  wcscat_s(maxCmd, batchFileBuilder.Path());
  wcscat_s(maxCmd, L"\"");
  SHELLEXECUTEINFOW sei;
  ZeroMemory(&sei, sizeof(sei));
  sei.cbSize = sizeof(sei);
  sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
  sei.lpVerb = exe.IsEnableAdministrator() ? L"runas" : L"open";
  sei.lpFile = L"cmd";
  sei.lpParameters = maxCmd;
  if (StringEndWith(exe.Executable(), L".exe") == 0) {
    sei.nShow = SW_HIDE;
  } else {
    sei.nShow = SW_SHOW;
  }
  if (ShellExecuteExW(&sei) == FALSE) {
    return GetLastError();
  }
  return 0;
}
