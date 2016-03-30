///
///
#include "Precompiled.h"
//
#include <Shlwapi.h>
#include <Shlobj.h>
///
#include "Environment.hpp"
//DoEnvironmentSubstW
//ExpandEnvironmentStringsW
//PathProcessCommand

bool AppLoaderEnvironment::Initialize(const wchar_t *alfile)
{
	std::wstring parent;
	if (PathIsRelativeW(alfile)) {

	}
	return true;
}