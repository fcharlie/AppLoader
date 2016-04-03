///
////
///
#ifndef APPLOADER_EXECUTABLE_HPP
#define APPLOADER_EXECUTABLE_HPP

#include <string>
#include <vector>

class ExecutableFile {
public:
  ExecutableFile() {
    ///
  }
  ~ExecutableFile() {
    ///
  }
  std::wstring &Executable() { return executable_; }
  const std::wstring &Executable() const { return executable_; }
  std::wstring &InitializeScript() { return initializeScript_; }
  const std::wstring &InitializeScript() const { return initializeScript_; }
  const std::wstring &StartupDir()const { return startupDir_; }
  std::wstring &StartupDir() { return startupDir_; }
  std::vector<std::wstring> &Path() { return path_; }
  const std::vector<std::wstring> &Path() const { return path_; }
  std::vector<std::wstring> &Args() { return args_; }
  const std::vector<std::wstring> &Args() const { return args_; }
  void AppContainerEnable(bool isEnable = true) { isAppContainer = isEnable; }
  bool IsEnableAppContainer() { return isAppContainer; }
  void AdministratorEnable(bool isEnable = true) { isAdministrator = isEnable; }
  bool IsEnableAdministrator() { return isAdministrator; }
  int Execute();

private:
  std::wstring executable_;
  std::wstring initializeScript_;
  std::wstring startupDir_;
  std::vector<std::wstring> path_;
  std::vector<std::wstring> args_;
  bool isAppContainer = false;
  bool isAdministrator = false;
};

#endif
