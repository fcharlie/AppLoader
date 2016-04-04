//
//
//
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <windows.h>
///
#include <AppLoader.h>
#include <AppLoaderFile.h>

///
#include "Executable.hpp"
#include "Execute.hpp"
#include "Environment.hpp"

void PrintUsage() {
  ///
}

void PrintVersion() {
  ///
}

enum AppLoaderPhase {
  kAppLoaderZero,
  kAppLoaderUsage,
  kAppLoaderVersion,
  kAppLoaderNormal,
};

struct AppLoaderPhaseCase {
  int state;
  std::vector<const wchar_t *> file;
};

int ParseArgv(int Argc, wchar_t **Argv, AppLoaderPhaseCase &Case) {
  const wchar_t *ArgX = nullptr;
  Case.state = kAppLoaderNormal;
  for (int i = 0; i < Argc; i++) {
    ArgX = Argv[i];
    if (ArgX == nullptr)
      break;
    if (ArgX[0] == '-') {
      if (_wcsicmp(ArgX, L"-h") == 0 || _wcsicmp(ArgX, L"--help") == 0) {
        Case.state = kAppLoaderUsage;
      } else if (_wcsicmp(ArgX, L"-v") == 0 ||
                 _wcsicmp(ArgX, L"--version") == 0) {
        Case.state = kAppLoaderVersion;
      } else if (_wcsicmp(ArgX, L"-f") == 0 || _wcsicmp(ArgX, L"--file") == 0) {
        if (i + 1 >= Argc) {
          Case.state = kAppLoaderZero;
          return 1;
        }
        i++;
        Case.file.push_back(Argv[i]);
        continue;
      } else {
        Case.state = kAppLoaderZero;
      }
      return 0;
    }
    Case.file.push_back(ArgX);
  }
  return 0;
}

class ArgvCase {
public:
  ArgvCase(LPWSTR *Argv) : Argv_(Argv) {}
  ~ArgvCase() {
    if (Argv_) {
      LocalFree(Argv_);
    }
  }
  LPWSTR *Argv() { return Argv_; }

private:
  LPWSTR *Argv_;
};

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
  int Argc = 0;
  wchar_t buf[4096];
  uint32_t counts;
  //return true;
  FasterResolveIcon(LR"(F:\Development\AppLoader\App.apploader)", buf, &counts);
  AppLoaderEnvironment appLoaderEnvironment;
  appLoaderEnvironment.Initialize(LR"(App.apploader)");
  ArgvCase argvCase = CommandLineToArgvW(GetCommandLineW(), &Argc);
  if (Argc < 2) {
    PrintUsage();
    return 0;
  }
  AppLoaderPhaseCase Case;
  ParseArgv(Argc - 1, argvCase.Argv() + 1, Case);
  switch (Case.state) {
  case kAppLoaderZero:
    PrintUsage();
    return 0;
  case kAppLoaderUsage:
    PrintUsage();
    return 0;
  case kAppLoaderVersion:
    PrintVersion();
    return 0;
  case kAppLoaderNormal:
    break;
  }
  for (auto &c : Case.file) {
    //// Parse C
    ExecutableFile executableFile;
    //....
    // executableFile.Execute();
  }
  return 0;
}
