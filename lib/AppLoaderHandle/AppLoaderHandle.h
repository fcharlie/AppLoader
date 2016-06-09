//
//
//
#pragma once
#include "Precompiled.h"
//{EECB5030-FB0F-4A54-BC4F-67C00CB10273}

class ATL_NO_VTABLE AppLoaderExtractIcon :
	public IPersistFile,
	public IExtractIconW {
public:
	HRESULT WINAPI GetIconLocation(
		UINT  uFlags,
		LPWSTR pszIconFile,
		UINT  cchMax,
		int   *piIndex,
		UINT  *pwFlags
		);
	HRESULT WINAPI Extract(
		LPCWSTR pszFile,
		UINT   nIconIndex,
		HICON  *phiconLarge,
		HICON  *phiconSmall,
		UINT   nIconSize
		);
};