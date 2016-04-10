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

class AppContainerContext {
public:
  // typedef std::vector<WELL_KNOWN_SID_TYPE> TCapabilityList;
  typedef std::vector<SID_AND_ATTRIBUTES> TCapabilitiesList;
  AppContainerContext() : appContainerSid(nullptr) {}
  ~AppContainerContext() {
    ///
    if (appContainerSid) {
      FreeSid(appContainerSid);
    }
    for (auto &c : mCapabilities) {
      if (c.Sid) {
        HeapFree(GetProcessHeap(), 0, c.Sid);
      }
    }
  }
  bool AppContainerContextInitialize() {
    if (!MakeWellKnownSIDAttributes()) {
      return false;
    }
    auto hr = DeleteAppContainerProfile(APPCONTAINER_PROFILE_NAME);
    if (::CreateAppContainerProfile(
            APPCONTAINER_PROFILE_NAME, APPCONTAINER_PROFILE_DISPLAYNAME,
            APPCONTAINER_PROFILE_DESCRIPTION,
            (mCapabilities.empty() ? NULL : mCapabilities.data()),
            (DWORD)mCapabilities.size(), &appContainerSid) != S_OK) {
      return false;
    }
    return true;
  }
  PSID GetAppContainerSid() const { return appContainerSid; }
  TCapabilitiesList &Capabilitis() { return mCapabilities; }

private:
  bool MakeWellKnownSIDAttributes() {
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
    for (auto c : capabilitiyTypeList) {
      PSID sid = HeapAlloc(GetProcessHeap(), 0, SECURITY_MAX_SID_SIZE);
      if (sid == nullptr) {
        return false;
      }
      DWORD sidListSize = SECURITY_MAX_SID_SIZE;
      if (::CreateWellKnownSid(c, NULL, sid, &sidListSize) == FALSE) {
        HeapFree(GetProcessHeap(), 0, sid);
        continue;
      }
      if (::IsWellKnownSid(sid, c) == FALSE) {
        HeapFree(GetProcessHeap(), 0, sid);
        continue;
      }
      SID_AND_ATTRIBUTES attr;
      attr.Sid = sid;
      attr.Attributes = SE_GROUP_ENABLED;
      mCapabilities.push_back(attr);
    }
    return true;
  }
  PSID appContainerSid;
  TCapabilitiesList mCapabilities;
};

BOOL WINAPI CreateAppContainerProcessEx(LPCWSTR lpApplication,
                                        LPCWSTR lpCommandline,
                                        LPCWSTR lpEvnironment,
                                        LPCWSTR lpDirectory) {
  AppContainerContext context;
  if (!context.AppContainerContextInitialize()) {
    return FALSE;
  }
  PROCESS_INFORMATION pi;
  STARTUPINFOEX siex = {sizeof(STARTUPINFOEX)};
  siex.StartupInfo.cb = sizeof(siex);
  SIZE_T cbAttributeListSize = 0;
  InitializeProcThreadAttributeList(NULL, 3, 0, &cbAttributeListSize);
  siex.lpAttributeList = (PPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(
      GetProcessHeap(), 0, cbAttributeListSize);
  BOOL bReturn = TRUE;
  if ((bReturn = InitializeProcThreadAttributeList(
           siex.lpAttributeList, 3, 0, &cbAttributeListSize)) == FALSE) {
    return FALSE;
  }
  SECURITY_CAPABILITIES sc;
  sc.AppContainerSid = context.GetAppContainerSid();
  sc.Capabilities =
      (context.Capabilitis().empty() ? NULL : context.Capabilitis().data());
  sc.CapabilityCount = static_cast<DWORD>(context.Capabilitis().size());
  sc.Reserved = 0;
  if ((bReturn = UpdateProcThreadAttribute(
           siex.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_SECURITY_CAPABILITIES,
           &sc, sizeof(sc), NULL, NULL)) == FALSE) {
	DeleteProcThreadAttributeList(siex.lpAttributeList);
    return FALSE;
  }
  auto len = wcslen(lpCommandline);
  std::vector<wchar_t> cmdlinebuf(len + 1);
  wmemcpy(cmdlinebuf.data(), lpCommandline, len);
  cmdlinebuf[len] = L'\0';
  bReturn =
      CreateProcessW(lpApplication, cmdlinebuf.data(), nullptr, nullptr, FALSE,
                     EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT,
                     (LPVOID)lpEvnironment, lpDirectory,
                     reinterpret_cast<STARTUPINFOW *>(&siex), &pi);
  if (bReturn) {
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
  }
  auto x = GetLastError();
  DeleteProcThreadAttributeList(siex.lpAttributeList);
  return bReturn;
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
