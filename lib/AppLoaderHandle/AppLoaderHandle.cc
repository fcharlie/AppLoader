/*
*
*/

// https://msdn.microsoft.com/en-us/library/windows/desktop/cc144122(v=vs.85).aspx

#include <stdio.h>
#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpReserved) {
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETCH:
    break;
  case DLL_PROCESS_DETCH:
    break;
  default:
    break;
  }
  /// To do
  return true;
}
