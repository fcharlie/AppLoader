///
///
#include "Precompiled.h"
#include <stdint.h>
//
#include "Executable.hpp"
#include "Execute.hpp"

/*
* PowerShell Aslo support UTF16 LE, so ,we save as WCHAR
*
*/
class PSFileBuilder {
public:
	PSFileBuilder()
	{
		//
	}
	~PSFileBuilder() {}
	bool Initialize()
	{
		wchar_t bom = 0xEFFF;
		WCHAR buf[MAX_PATH];
		auto dwRetVal = GetTempPathW(MAX_PATH, buf);
		if (dwRetVal > MAX_PATH || dwRetVal == 0) {
			return false;
		}
		path_.assign(buf);
		auto uRetVal = GetTempFileNameW(path_.data(), L"AppLoader", 0, buf);
		if (uRetVal == 0)
			return false;
		path_.assign(buf);
		path_.append(L".ps1");
		/// File Set size 0
		return true;
	}
	 uint32_t Write(const wchar_t *text, uint32_t size)
	{
		DWORD dwBytes=0;
		if (WriteFile(hFile, text, size*sizeof(wchar_t), &dwBytes, NULL)) {
			return dwBytes / 2;
		}
		return 0;
	}
	const wchar_t *Path()
	{
		return path_.data();
	}
private:
	HANDLE hFile;
	std::wstring path_;
};


//Start-Process -NoNewWindow -FilePath $cmd -Verb RunAs

/*
$env:PATH="$env:PATH;/path/to/path"
Invoke-Expression /path/to/initializeScript.ps1

Start-Process -NoNewWindow -FilePath $cmd -Verb RunAs -Argument "some"

*/

int PowerShellAttachExecute(const ExecutableFile &exe)
{
	return 0;
}
