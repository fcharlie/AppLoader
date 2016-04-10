//
//
#ifndef APPLOADER_EXECUTE_HPP
#define APPLOADER_EXECUTE_HPP
//// To Be
#include <string>
#include <vector>

#ifndef APPLOADER_EXECUTABLE_HPP
#include "Executable.hpp"
#endif

BOOL WINAPI CreateElevatedProcess(LPCWSTR lpFile, LPCWSTR lpAgrs,
                                  LPCWSTR lpEnvironment, LPCWSTR lpDirectory);
//
BOOL WINAPI CreateAppContainerProcessEx(LPCWSTR lpApplication,
                                        LPWSTR lpCommandline,
                                        LPCWSTR lpEvnironment,
                                        LPCWSTR lpDirectory);
int AppContainerExecute(const ExecutableFile &exe);
int PowerShellAttachExecute(const ExecutableFile &exe);
int BatchAttachExecute(const ExecutableFile &exe);
int ProcessExecute(const ExecutableFile &file);

#endif
