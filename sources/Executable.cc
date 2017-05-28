///
///
///
///
#include "Precompiled.h"
///
#include <algorithm>
#include <regex>
//
#include "AppLoaderFile.h"
#include "Environment.hpp"
#include "Executable.hpp"
#include "Execute.hpp"

bool StringEndWith(const std::wstring &s, const wchar_t *subfix) {
  auto l = wcslen(subfix);
  if (s.size() < l)
    return false;
  if (s.compare(s.size() - l, l, subfix) == 0)
    return true;
  return false;
}

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
  if (isAppContainer) {
    return AppContainerExecute(*this);
  }
  if (initializeScript_.size() > 0) {
    if (IsMatchBatch(initializeScript_)) {
      /// to execute Batch
      return BatchAttachExecute(*this);
    } else if (IsMatchPowerShell(initializeScript_)) {
      /// to execute PowerShell
      return PSAttachExecute(*this);
    }
  }
  return ProcessExecute(*this);
}

bool ExecutableFile::Deserialize(const std::wstring &appfile,
                                           ExecutableFile &exe) {
  AppLoaderEnvironment appenv;
  appenv.Initialize(appfile.data());
  /*
   TO init
  */
  AppLoaderFile file(appfile.c_str());
  if (!file.Parse())
    return false;
  auto iter = file.Objects().begin();
  //// Exe Name
  if ((iter = file.At(L"Base.Executable")) != file.ObjectEnd()) {
    if (iter->second.IsString()) {
      exe.executable_.assign(iter->second.AsString());
    }
  }

  if ((iter = file.At(L"Base.InitializeScript")) != file.ObjectEnd()) {
    if (iter->second.IsString()) {
      exe.initializeScript_.assign(iter->second.AsString());
    }
  }
  if ((iter = file.At(L"Base.StartupDir")) != file.ObjectEnd()) {
    if (iter->second.IsString()) {
      exe.startupDir_.assign(iter->second.AsString());
    }
  }
  ///// Boolean value set
  if ((iter = file.At(L"Base.IsAdministrator")) != file.ObjectEnd()) {
    if (iter->second.IsBoolean()) {
      exe.isAdministrator = iter->second.AsBoolean();
    }
  }
  if ((iter = file.At(L"Base.IsAppContainer")) != file.ObjectEnd()) {
    if (iter->second.IsBoolean()) {
      exe.isAppContainer = iter->second.AsBoolean();
    }
  }
  if ((iter = file.At(L"Base.IsClearEnvironment")) != file.ObjectEnd()) {
    if (iter->second.IsBoolean()) {
      exe.isClearEnvironment = iter->second.AsBoolean();
    }
  }

  //// Array
  if ((iter = file.At(L"Base.Path")) != file.ObjectEnd()) {
    if (iter->second.IsArray()) {
      auto obj = iter->second;
      for (auto &o : obj) {
		  if (o.IsString()&&o.AsString().size()) {
          exe.path_.push_back(o.AsString());
        }
      }
    }
  }
  if ((iter = file.At(L"Base.Args")) != file.ObjectEnd()) {
    if (iter->second.IsArray()) {
      auto obj = iter->second;
      for (auto &o : obj) {
        if (o.IsString()&&o.AsString().size()) {
          exe.args_.push_back(o.AsString());
        }
      }
    }
  }

  appenv.DoEnvironmentSubstW(exe.executable_);
  appenv.DoEnvironmentSubstW(exe.startupDir_);
  appenv.DoEnvironmentSubstW(exe.initializeScript_);
  return true;
}
