///
///
#include "Precompiled.h"
#include <stdint.h>
//
#include "Environment.hpp"
#include "Executable.hpp"
#include "Execute.hpp"

/*
* PowerShell Aslo support UTF16 LE, so ,we save as WCHAR
*
*/
PScriptFileBuilder::~PScriptFileBuilder() {
  if (hFile && hFile != INVALID_HANDLE_VALUE) {
    CloseHandle(hFile);
  }
  if (initialized = true) {
    DeleteFileW(path_.data());
  }
}

bool PScriptFileBuilder::Initialize() {
  WORD bom = 0xFEFF;
  WCHAR buf[MAX_PATH];
  auto dwRetVal = GetTempPathW(MAX_PATH, buf);
  if (dwRetVal > MAX_PATH || dwRetVal == 0) {
    return false;
  }
  path_.assign(buf);
  swprintf_s(buf, L"%sAppLoader.%d.ps1", path_.data(), GetCurrentThreadId());
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
  DWORD dwWrite = 0;
  WriteFile(hFile, &bom, sizeof(bom), &dwWrite, NULL);
  return true;
}
////
uint32_t PScriptFileBuilder::Write(const wchar_t *text, uint32_t size) {
  DWORD dwBytes = 0;
  if (WriteFile(hFile, text, size * sizeof(wchar_t), &dwBytes, NULL)) {
    return dwBytes / 2;
  }
  return 0;
}
void PScriptFileBuilder::Flush() {
  CloseHandle(hFile);
  hFile = nullptr;
}

// Start-Process -NoNewWindow -FilePath $cmd -Verb RunAs

/*
$env:PATH="$env:PATH;/path/to/path"
Invoke-Expression /path/to/initializeScript.ps1

Start-Process -NoNewWindow -FilePath $cmd -Verb RunAs -Argument "some"

*/

int PSAttachExecute(const ExecutableFile &exe) {
  PScriptFileBuilder psbuilder;
  if (!psbuilder.Initialize()) {
    return FALSE;
  }
  if (exe.Path().size()) {
    std::wstring paths(L"$env:PATH=");
    for (auto &p : exe.Path()) {
      paths.append(p);
      paths.push_back(';');
    }
    if (exe.IsClearEnvironment()) {
      std::wstring def;
      EnvironmentPathBuilder(def);
      paths.append(def);
    } else {
      paths.append(L"${env:PATH}");
    }
    paths.append(L" \n");
  }
  WCHAR cmdlet[PATHCCH_MAX_CCH];
  swprintf_s(cmdlet, L"Invoke-Express \"%s\"\n"
                     L"Start-Process -Verb runas -FilePath \"%s\" ",
             exe.InitializeScript().data(), exe.Executable().data());
  if (exe.Args().size()) {
    wcscat_s(cmdlet, L" -ArgumentList \"");
    for (auto &a : exe.Args()) {
      if (a.find(L' ') < a.size()) {
        wcscat_s(cmdlet, L"`\"");
        wcscat_s(cmdlet, a.data());
        wcscat_s(cmdlet, L"`\" ");
      } else {
        wcscat_s(cmdlet, a.data());
        wcscat_s(cmdlet, L" ");
      }
    }
    wcscat_s(cmdlet, L"\"");
  }
  psbuilder.Write(cmdlet, wcslen(cmdlet));
  psbuilder.Flush();
  swprintf_s(cmdlet,
             L"PowerShell -NoLogo -ExecutionPolicy unrestricted  -File \"%s\"",
             psbuilder.Path());
  STARTUPINFOW si;
  PROCESS_INFORMATION pi;
  SecureZeroMemory(&si, sizeof(si));
  SecureZeroMemory(&pi, sizeof(pi));
  si.cb = sizeof(si);
  auto result = CreateProcessW(
      nullptr, cmdlet, nullptr, nullptr, FALSE,
      CREATE_UNICODE_ENVIRONMENT | CREATE_NO_WINDOW, nullptr,
      exe.StartupDir().empty() ? nullptr : exe.StartupDir().data(), &si, &pi);
  if (result) {
    WaitForSingleObject(pi.hProcess, 10000);
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    if (exitCode != 0) {
      result = FALSE;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
  }
  return TRUE;

  return 0;
}
