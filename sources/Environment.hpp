///
///
#ifndef APPLOADER_ENVIRONMENT_HPP
#define APPLOADER_ENVIRONMENT_HPP
#include <string>
#include <list>
#include <cstdint>
#include <initializer_list>
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
	enum {
		kEnvironmentUnicodeString = CREATE_UNICODE_ENVIRONMENT
	};
	//enum EnvironmentStringsFlags {
	//	kEnvironmentUnicodeString
	//}EnvironemtFlags;
	AppLoaderEnvironmentStrings();
	~AppLoaderEnvironmentStrings();
	bool InitializeEnvironment();
	const wchar_t *EnvironmentBuilder();
	bool Delete(const wchar_t *key);
	bool Append(const wchar_t *key,const std::wstring &va);
	bool Insert(const wchar_t *key, const std::wstring &va);
	bool Replace(const wchar_t *key, const std::wstring &va);
private:
	std::list<std::wstring> envlist;
	wchar_t *envbuf;
	uint32_t updateCount=0;
	uint32_t lastBuilderCount=0;
};

/*
* Default PATH
* C:\Windows\System32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0
*/

///
// Build PATH Environment Variable
///
bool EnvironmentPathBuilder(std::wstring &paths);

/*
* AppLoaderEnvironment , Allow AppLoader File support environment variables  
* AppRoot, AppLoader.apploader location directory
*/
class AppLoaderEnvironment {
public:
	AppLoaderEnvironment();
	bool Initialize(const wchar_t *alfile);
	bool DoEnvironmentSubst(std::wstring &str);
	bool QueryEnvironmentVariableU(const std::wstring &key,std::wstring &value);
private:
	std::unordered_map<std::wstring, std::wstring > builtEnv;
};

#endif
