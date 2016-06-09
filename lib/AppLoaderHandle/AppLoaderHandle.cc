/*
*
*/

// https://msdn.microsoft.com/en-us/library/windows/desktop/cc144122(v=vs.85).aspx

#include "Precompiled.h"
#include "AppLoaderHandle.h"

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpReserved) {
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    break;
  default:
    break;
  }
  /// To do
  return true;
}

/*
DllGetClassObject()
DllCanUnloadNow()
DllRegisterServer()
DllUnregisterServer()
*/

// Used to determine whether the DLL can be unloaded by OLE.
STDAPI DllCanUnloadNow(void)
{

	//return _AtlModule.DllCanUnloadNow();
	return S_OK;
}

// Returns a class factory to create an object of the requested type.
STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID* ppv)
{
	//return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
	return S_OK;
}

// DllRegisterServer - Adds entries to the system registry.
STDAPI DllRegisterServer(void)
{
	// registers object, typelib and all interfaces in typelib
	//HRESULT hr = _AtlModule.DllRegisterServer();
	return S_OK;
}

// DllUnregisterServer - Removes entries from the system registry.
STDAPI DllUnregisterServer(void)
{
	//HRESULT hr = _AtlModule.DllUnregisterServer();
	return S_OK;
}

// DllInstall - Adds/Removes entries to the system registry per user per machine.
STDAPI DllInstall(BOOL bInstall, _In_opt_  LPCWSTR pszCmdLine)
{
	//HRESULT hr = E_FAIL;
	//static const wchar_t szUserSwitch[] = L"user";

	//if (pszCmdLine != NULL) {
	//	if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0) {
	//		ATL::AtlSetPerUserRegistration(true);
	//	}
	//}

	//if (bInstall) {
	//	hr = DllRegisterServer();
	//	if (FAILED(hr)) {
	//		DllUnregisterServer();
	//	}
	//} else {
	//	hr = DllUnregisterServer();
	//}

	return S_OK;
}


HRESULT WINAPI AppLoaderExtractIcon::GetIconLocation(
	UINT  uFlags,
	LPWSTR pszIconFile,
	UINT  cchMax,
	int   *piIndex,
	UINT  *pwFlags
	)
{
	return S_FALSE;
}
HRESULT WINAPI AppLoaderExtractIcon::Extract(
	LPCWSTR pszFile,
	UINT   nIconIndex,
	HICON  *phiconLarge,
	HICON  *phiconSmall,
	UINT   nIconSize
	)
{
	return S_FALSE;
}