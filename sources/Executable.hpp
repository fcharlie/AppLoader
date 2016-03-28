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
  std::vector<std::wstring> &Path() { return path_; }
  const std::vector<std::wstring> &Path() const { return path_; }
  std::vector<std::wstring> &Args() { return args_; }
  const std::vector<std::wstring> &Args() const { return args_; }
  int Execute();

private:
  std::wstring executable_;
  std::wstring initializeScript_;
  std::vector<std::wstring> path_;
  std::vector<std::wstring> args_;
};

#endif
