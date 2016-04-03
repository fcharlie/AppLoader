///
///
#ifndef APPLOADER_ENVIRONMENT_HPP
#define APPLOADER_ENVIRONMENT_HPP
#include <string>
#include <unordered_map>

/*
Run is this AppLoaderFile Path
*/
//struct AppLoaderEnvironmentFlags {
//
//};

class AppLoaderEnvironment {
public:
	AppLoaderEnvironment();
	bool Initialize(const wchar_t *alfile);
	bool DoEnvironmentSubst(std::wstring &str);
	bool ExpendEnvironment(const std::wstring &key,std::wstring &value);
private:
	std::unordered_map<std::wstring, std::wstring > builtEnv;
};

#endif
