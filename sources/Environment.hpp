///
///
#ifndef APPLOADER_ENVIRONMENT_HPP
#define APPLOADER_ENVIRONMENT_HPP
#include <cstdint>
#include <initializer_list>
#include <list>
#include <string>
#include <unordered_map>


// max # of characters we support using the "\\?\" syntax
// (0x7FFF + 1 for NULL terminator)
#ifndef PATHCCH_MAX_CCH
#define PATHCCH_MAX_CCH
#endif

#ifndef CREATE_UNICODE_ENVIRONMENT
#define CREATE_UNICODE_ENVIRONMENT 0x00000400
#endif

/*
* Create Child Process Environment Strings
*/
class AppLoaderEnvironmentStrings {
public:
  enum { kEnvironmentUnicodeString = CREATE_UNICODE_ENVIRONMENT };
  // enum EnvironmentStringsFlags {
  //	kEnvironmentUnicodeString
  //}EnvironemtFlags;
  AppLoaderEnvironmentStrings();
  ~AppLoaderEnvironmentStrings();
  bool InitializeEnvironment();
  const wchar_t *EnvironmentBuilder();
  bool Delete(const wchar_t *key);
  bool Append(const wchar_t *key, const std::wstring &va);
  bool Insert(const wchar_t *key, const std::wstring &va);
  bool Replace(const wchar_t *key, const std::wstring &va);

private:
  std::list<std::wstring> envlist;
  wchar_t *envbuf;
  uint32_t updateCount = 0;
  uint32_t lastBuilderCount = 0;
};

/*
* Default PATH
* C:\Windows\System32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0
*/

///
// Build PATH Environment Variable
///
bool EnvironmentPathBuilder(std::wstring &paths);

class ArgvToCommandlineBuilder {
public:
  ~ArgvToCommandlineBuilder() {
    if (Args_) {
      free(Args_);
    }
  }
  bool Initialize(const std::wstring &path,
                  const std::vector<std::wstring> &Argv) {
    std::wstring wcmd;
    if (path.empty() && Argv.empty()) {
      Args_ = nullptr;
      return false;
    }
    if (path.size()) {
      if (path.find(L" ") < path.size()) {
        wcmd.assign(L"\"");
        wcmd.append(path);
        wcmd.append(L"\" ");
      } else {
        wcmd.assign(path);
        wcmd.push_back(' ');
      }
    }
    for (auto &s : Argv) {
      if (s.find(' ') < s.size()) {
        wcmd.push_back('"');
        wcmd.append(s);
        wcmd.append(L"\" ");
      } else {
        wcmd.append(s);
        wcmd.push_back(' ');
      }
    }
    Args_ = _wcsdup(wcmd.data());
    return (Args_ != nullptr); /// if failed out of memory
  }
  wchar_t *Args() { return this->Args_; }

private:
  wchar_t *Args_;
};

enum ArgvCombineFlags { kArgvPowerShell, kArgvBatch, kArgvNative };

bool ArgvCombine(const std::vector<std::wstring> &argv, std::wstring &args,
                 int flags);
bool PathsCombine(const std::vector<std::wstring> &pathv, std::wstring &paths);
BOOL WINAPI IsAdministrator();

/*
* AppLoaderEnvironment , Allow AppLoader File support environment variables
* AppRoot, AppLoader.apploader location directory
*/
class AppLoaderEnvironment {
public:
  AppLoaderEnvironment();
  bool Initialize(const wchar_t *alfile);
  bool DoEnvironmentSubst(std::wstring &str);
  bool QueryEnvironmentVariableU(const std::wstring &key, std::wstring &value);

private:
  std::unordered_map<std::wstring, std::wstring> builtEnv;
};

bool EnvironmentResolvePathEx(AppLoaderEnvironmentStrings &aes,
                              const std::vector<std::wstring> &paths,
                              bool isclean = false);

#endif
