//
//
//
#include "Precompiled.h"
//
#include <Shellapi.h>
//
#include "Environment.hpp"
#include "Executable.hpp"
#include "Execute.hpp"

class WideStringCopy {
public:
  WideStringCopy(const wchar_t *buf) { ptr_ = _wcsdup(buf); }
  ~WideStringCopy() {
    if (ptr_) {
      free(ptr_);
    }
  }
  wchar_t *Ptr() { return ptr_; }

private:
  wchar_t *ptr_;
};

BOOL WINAPI CreateElevatedProcess(LPCWSTR lpFile, LPCWSTR lpArgs,
                                  LPCWSTR lpEnvironment, LPCWSTR lpDirectory) {
  /// powershell -Command "&{ Start-Process -Verb runas -FilePath app.exe ....}"
  std::wstring cmdline(L"PowerShell -Command \"&{ Start-Process -Verb runas ");
  cmdline.append(L"-FilePath \"");
  cmdline.append(lpFile);
  cmdline.append(L"\" ");
  if (lpArgs) {
    cmdline.append(L" -ArgumentList `\"");
    cmdline.append(lpArgs);
    cmdline.append(L"`\" ");
  }
  cmdline.append(L"}\"");
  STARTUPINFOW si;
  PROCESS_INFORMATION pi;
  SecureZeroMemory(&si, sizeof(si));
  SecureZeroMemory(&pi, sizeof(pi));
  si.cb = sizeof(si);
  WideStringCopy wcmd(cmdline.c_str());
  auto result = CreateProcessW(nullptr, wcmd.Ptr(), nullptr, nullptr, FALSE,
                               CREATE_UNICODE_ENVIRONMENT | CREATE_NO_WINDOW,
                               (LPVOID)lpEnvironment, lpDirectory, &si, &pi);
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
  return result;
}

int ProcessExecute(const ExecutableFile &file) {
  //
  AppLoaderEnvironmentStrings aes;
  EnvironmentResolvePathEx(aes, file.Path(), file.IsClearEnvironment());
  BOOL result = FALSE;
  if (file.IsEnableAdministrator() && !IsAdministrator()) {
    std::wstring args;
    ArgvCombine(file.Args(), args, kArgvPowerShell);
    result = CreateElevatedProcess(
        file.Executable().data(), args.empty() ? nullptr : args.data(),
        aes.EnvironmentBuilder(),
        file.StartupDir().empty() ? nullptr : file.StartupDir().data());
  } else {
    ArgvToCommandlineBuilder argvBuilder;
    PROCESS_INFORMATION pi;
    STARTUPINFOW si = {sizeof(STARTUPINFOW)};
    auto lpDirectory = file.StartupDir().empty() ? nullptr : file.StartupDir().data();
    argvBuilder.Initialize(file.Executable(), file.Args());
    CreateProcessW(nullptr, argvBuilder.Args(), nullptr, nullptr, FALSE,
                   CREATE_UNICODE_ENVIRONMENT,       ///
                   (LPVOID)aes.EnvironmentBuilder(), ////
                   lpDirectory, &si, &pi);
  }
  if (!result)
    return GetLastError();
  return 0;
}