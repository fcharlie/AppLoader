//
//
#ifndef APPLOADER_EXECUTE_HPP
#define APPLOADER_EXECUTE_HPP
//// To Be
#include <string>
#include <vector>
#include <stdint.h>

#ifndef APPLOADER_EXECUTABLE_HPP
#include "Executable.hpp"
#endif

class PScriptFileBuilder {
public:
  PScriptFileBuilder() : hFile(nullptr) {
    //
  }
  ~PScriptFileBuilder();
  bool Initialize();
  uint32_t Write(const wchar_t *text, uint32_t size);
  const wchar_t *Path() { return path_.data(); }
  void Flush();
private:
  HANDLE hFile;
  std::wstring path_;
  bool initialized = false;
};

BOOL WINAPI PSProcessElevatedBuilder(LPCWSTR lpFile, const std::vector<std::wstring> &vArgs,
                                     LPCWSTR lpEnvironment,
                                     LPCWSTR lpDirectory);
//
BOOL WINAPI CreateAppContainerProcessEx(LPCWSTR lpApplication,
                                        LPWSTR lpCommandline,
                                        LPCWSTR lpEvnironment,
                                        LPCWSTR lpDirectory);
int AppContainerExecute(const ExecutableFile &exe);
int PSAttachExecute(const ExecutableFile &exe);
int BatchAttachExecute(const ExecutableFile &exe);
int ProcessExecute(const ExecutableFile &file);

#endif
