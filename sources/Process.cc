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

BOOL WINAPI PSProcessElevatedBuilder(LPCWSTR lpFile,
                                     const std::vector<std::wstring> &vArgs,
                                     LPCWSTR lpEnvironment,
                                     LPCWSTR lpDirectory) {
  PScriptFileBuilder psbuilder;
  if (!psbuilder.Initialize()) {
    return FALSE;
  }
  WCHAR cmdlet[PATHCCH_MAX_CCH];
  swprintf_s(cmdlet, L"Start-Process -Verb runas -FilePath \"%s\" ", lpFile);
  if (vArgs.size()) {
    wcscat_s(cmdlet, L" -ArgumentList \"");
    for (auto &a : vArgs) {
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
  auto result = CreateProcessW(nullptr, cmdlet, nullptr, nullptr, FALSE,
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
  return TRUE;
}

int ProcessExecute(const ExecutableFile &file) {
  AppLoaderEnvironmentStrings aes;
  EnvironmentResolvePathEx(aes, file.Path(), file.IsClearEnvironment());
  BOOL result = FALSE;
  if (file.IsEnableAdministrator() && !IsAdministrator()) {
    result = PSProcessElevatedBuilder(
        file.Executable().data(), file.Args(), aes.EnvironmentBuilder(),
        file.StartupDir().empty() ? nullptr : file.StartupDir().data());
  } else {
    ArgvToCommandlineBuilder argvBuilder;
    PROCESS_INFORMATION pi;
    STARTUPINFOW si = {sizeof(STARTUPINFOW)};
    auto lpDirectory =
        file.StartupDir().empty() ? nullptr : file.StartupDir().data();
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