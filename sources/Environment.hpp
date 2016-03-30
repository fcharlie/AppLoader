///
///
#ifndef APPLOADER_ENVIRONMENT_HPP
#define APPLOADER_ENVIRONMENT_HPP
#include <string>
#include <unordered_map>

/*
Run is this AppLoaderFile Path
*/


class AppLoaderEnvironment {
public:
	AppLoaderEnvironment();
	bool Initialize(const wchar_t *alfile);
private:
	std::unordered_map<const wchar_t *, std::wstring > builtEnv;
};

#endif
