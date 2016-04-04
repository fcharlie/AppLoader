/*
*
*
*
*/
#include "Precompiled.h"
/////
#include <ShlObj.h>
#include <Userenv.h>
#include <functional>
#include <memory>
#include <type_traits>
#include <vector>

////
#include "Environment.hpp"
#include "Executable.hpp"
#include "Execute.hpp"

#define APPCONTAINER_PROFILE_NAME L"Force.Charlie.AppLoader.AppContainer"
#define APPCONTAINER_PROFILE_DISPLAYNAME L"Force.Charlie.AppLoader.AppContainer"
#define APPCONTAINER_PROFILE_DESCRIPTION L"AppLoader AppContainer"

class DArray {
private:
  wchar_t *mPtr;

public:
  DArray(const wchar_t *cwstr) : mPtr(nullptr) {
    if (cwstr) {
      mPtr = _wcsdup(cwstr);
    }
  }
  ~DArray() {
    if (mPtr)
      free(mPtr);
  }
  wchar_t *get() { return this->mPtr; }
};

typedef std::shared_ptr<std::remove_pointer<PSID>::type> SHARED_SID;
static bool SetCapability(const WELL_KNOWN_SID_TYPE type,
                          std::vector<SID_AND_ATTRIBUTES> &list,
                          std::vector<SHARED_SID> &sidList) {
  SHARED_SID capabilitySid(new unsigned char[SECURITY_MAX_SID_SIZE]);
  DWORD sidListSize = SECURITY_MAX_SID_SIZE;
  if (::CreateWellKnownSid(type, NULL, capabilitySid.get(), &sidListSize) ==
      FALSE) {
    return false;
  }
  if (::IsWellKnownSid(capabilitySid.get(), type) == FALSE) {
    return false;
  }
  SID_AND_ATTRIBUTES attr;
  attr.Sid = capabilitySid.get();
  attr.Attributes = SE_GROUP_ENABLED;
  list.push_back(attr);
  sidList.push_back(capabilitySid);
  return true;
}

static bool
MakeWellKnownSIDAttributes(std::vector<SID_AND_ATTRIBUTES> &capabilities,
                           std::vector<SHARED_SID> &capabilitiesSidList) {

  const WELL_KNOWN_SID_TYPE capabilitiyTypeList[] = {
      WinCapabilityInternetClientSid,
      WinCapabilityInternetClientServerSid,
      WinCapabilityPrivateNetworkClientServerSid,
      WinCapabilityPicturesLibrarySid,
      WinCapabilityVideosLibrarySid,
      WinCapabilityMusicLibrarySid,
      WinCapabilityDocumentsLibrarySid,
      WinCapabilitySharedUserCertificatesSid,
      WinCapabilityEnterpriseAuthenticationSid,
      WinCapabilityRemovableStorageSid,
  };
  for (auto type : capabilitiyTypeList) {
    if (!SetCapability(type, capabilities, capabilitiesSidList)) {
      return false;
    }
  }
  return true;
}

HRESULT LauncherWithAppContainerEx(LPCWSTR pszApp, LPCWSTR cmdArgs,
                                   LPCWSTR workDir, DWORD &pid) {
  PSID appContainerSid;
  HANDLE hPrimaryToken = nullptr;
  auto hr = DeleteAppContainerProfile(APPCONTAINER_PROFILE_NAME);
  /// AppContainerProfile
  if (hr != S_OK) {
    return S_FALSE;
  }
  std::vector<SID_AND_ATTRIBUTES> capabilities;
  std::vector<SHARED_SID> capabilitiesSidList;
  if (!MakeWellKnownSIDAttributes(capabilities, capabilitiesSidList)) {
    return S_FALSE;
  }
  hr = ::CreateAppContainerProfile(
      APPCONTAINER_PROFILE_NAME, APPCONTAINER_PROFILE_DISPLAYNAME,
      APPCONTAINER_PROFILE_DESCRIPTION,
      (capabilities.empty() ? NULL : &capabilities.front()),
      (DWORD)capabilities.size(), &appContainerSid);
  if (hr != S_OK)
    return S_FALSE;
  DArray cmdArgsX(cmdArgs);
  PROCESS_INFORMATION pi;
  STARTUPINFOEX siex = {sizeof(STARTUPINFOEX)};
  ZeroMemory(&siex.StartupInfo, sizeof(STARTUPINFOW));
  siex.StartupInfo.cb = sizeof(STARTUPINFOEXW);
  SIZE_T cbAttributeListSize = 0;
  InitializeProcThreadAttributeList(NULL, 3, 0, &cbAttributeListSize);
  siex.lpAttributeList = (PPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(
      GetProcessHeap(), 0, cbAttributeListSize);
  BOOL bReturn = TRUE;
  if ((bReturn = InitializeProcThreadAttributeList(
           siex.lpAttributeList, 3, 0, &cbAttributeListSize)) == FALSE) {
    hr = GetLastError();
    goto Cleanup;
  }
  SECURITY_CAPABILITIES sc;
  sc.AppContainerSid = appContainerSid;
  sc.Capabilities = (capabilities.empty() ? NULL : &capabilities.front());
  sc.CapabilityCount = static_cast<DWORD>(capabilities.size());
  sc.Reserved = 0;
  if ((bReturn = UpdateProcThreadAttribute(
           siex.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_SECURITY_CAPABILITIES,
           &sc, sizeof(sc), NULL, NULL)) == FALSE) {
    hr = GetLastError();
    goto Cleanup;
  }
  if ((bReturn = CreateProcessW(pszApp, cmdArgsX.get(), nullptr, nullptr, FALSE,
                                EXTENDED_STARTUPINFO_PRESENT, nullptr, workDir,
                                reinterpret_cast<LPSTARTUPINFOW>(&siex),
                                &pi)) == FALSE) {
    hr = GetLastError();
    goto Cleanup;
  }
  pid = GetProcessId(pi.hProcess);
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
Cleanup:
  DeleteProcThreadAttributeList(siex.lpAttributeList);
  FreeSid(appContainerSid);
  return hr;
}

int AppContainerExecute(const ExecutableFile &exe) {
  AppLoaderEnvironmentStrings aes;
  if (aes.InitializeEnvironment()) {
    return 1;
  }
  if (exe.Path().size() > 0) {
	  std::wstring path_;
	  for (auto &p : exe.Path()) {
		  path_.append(p);
		  path_.push_back(';');
	  }
	  if (exe.IsClearEnvironment()) {
		  std::wstring paths;
		  if (!EnvironmentPathBuilder(paths))
			  return 1;
		  paths.append(path_);
		  aes.Replace(L"PATH", paths);
	  } else {
		  aes.Insert(L"PATH", path_);
	  }
  }
  /// TO USE Create Environment
  return 0;
}
