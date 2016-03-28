//
//
//
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <windows.h>
#include <AppLoader.h>
#include <AppLoaderFile.h>

int WINAPI wWinMain(HINSTANCE ,HINSTANCE, LPWSTR ,int)
{
	int Argc = 0;
	LPWSTR *Argv = CommandLineToArgvW(GetCommandLineW(), &Argc);
	LocalFree(Argv);
	wchar_t buffer[260] = { 0 };
	//AppLoaderExtraIcon(LR"(F:\Development\AppLoader\App.apploader)", buffer,260);
	//MessageBoxW(nullptr, buffer, L"Test", MB_OK);
	return 0;
}
