//
//
#ifndef APPLOADER_EXECUTE_HPP
#define APPLOADER_EXECUTE_HPP
//// To Be
#ifndef APPLOADER_EXECUTABLE_HPP
#include "Executable.hpp"
#endif

int AppContainerExecute(const ExecutableFile &exe);
int PowerShellAttachExecute(const ExecutableFile &exe);
int BatchAttachExecute(const ExecutableFile &exe);
int NativeProcessExecute(const ExecutableFile &file);

#endif
