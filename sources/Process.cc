//
//
//
#include "Precompiled.h"
//
#include <Shellapi.h>
//
#include "Executable.hpp"
#include "Execute.hpp"

class WideStringCopy {
public:
	WideStringCopy(const wchar_t *buf)
	{
		ptr_ = _wcsdup(buf);
	}
	~WideStringCopy()
	{
		if (ptr_) {
			free(ptr_);
		}
	}
	wchar_t *Ptr()
	{
		return ptr_;
	}
private:
	wchar_t *ptr_;
};

class BideProcess {
public:
	bool Initialize()
	{
		SHELLEXECUTEINFOW sei;
		ZeroMemory(&sei, sizeof(sei));
		sei.lpFile = L"cmd.exe";
		sei.cbSize = sizeof(sei);
		sei.lpParameters = L"/k";
		sei.lpVerb = L"runas";
		sei.nShow = SW_HIDE;
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		if (!ShellExecuteExW(&sei)) {
			return false;
		}
		hProcess = sei.hProcess;
		return true;
	}
	bool DuplicateElevateProcessToken(PHANDLE hPNewHandle)
	{
		return true;
	}
	~BideProcess()
	{
		if (hProcess) {
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
		}
	}
private:
	HANDLE hProcess;
};

BOOL WINAPI ElevateCreateProcess(LPCWSTR lpFile,LPCWSTR lpAgrs,LPCWSTR lpEnvironment,LPCWSTR lpDirectory)
{

	return TRUE;
}

int ProcessExecute(const ExecutableFile &file)
{
	return 0;
}