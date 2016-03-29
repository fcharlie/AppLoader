///
///
///
///
#include <algorithm>
#include <regex>

#include "Executable.hpp"
#include "Execute.hpp"

inline bool IsMatchBatch(const std::wstring &script) {
  // [.](bat|cmd)$
  std::wregex pattern(LR"([.](bat|cmd)$)");
  return std::regex_match(script, pattern);
}

inline bool IsMatchPowerShell(const std::wstring &script) {
  std::wregex pattern(LR"([.](ps1)$)");
  return std::regex_match(script, pattern);
}

int ExecutableFile::Execute() {
  if (initializeScript_.size() > 0) {
    if (IsMatchBatch(initializeScript_)) {
      /// to execute Batch
      return BatchAttachExecute(*this);
    } else if (IsMatchPowerShell(initializeScript_)) {
      /// to execute PowerShell
      return PowerShellAttachExecute(*this);
    }
  }
  if (isAppContainer) {
    return AppContainerExecute(*this);
  }
  return NativeProcessExecute(*this);
}
