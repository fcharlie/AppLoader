///
////
///
///
///

#include <AppLoaderFile.h>

class StringInput {
public:
	StringInput() :hFile(nullptr)
	{

	}
	~StringInput()
	{
		if (hFile) {
			CloseHandle(hFile);
		}
	}
private:
	HANDLE hFile;
};

bool AppLoaderFile::Parse() { 
	//
	return true; 
}

bool FasterResolveIcon(const wchar_t *file, wchar_t *receive, uint32_t *counts){
  if (file == nullptr) {
    return false;
  }
  WCHAR buffer[8192] = { 0 };
  DWORD bytes = 0;
  HANDLE hFile = CreateFileW(file, GENERIC_READ, FILE_SHARE_READ, NULL,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == NULL)
    return false;
  if (!ReadFile(hFile, buffer, 8192 * 2, &bytes, NULL)) {
	  return false;
  }
  auto n = bytes / 2;
  wchar_t *p = buffer;
  buffer[n] = L'\0';
  for (auto i = 1; i < n; i++) {
	  if (buffer[i] == L'['&&buffer[i - 1] == '\n') {
		  if (wcscmp(&buffer[i], L"[Base]\r\n") == 0) {
			  p = &buffer[i] + 8;
			  break;
		  }
		  if (wcscmp(&buffer[i], L"[Base]\n") == 0) {
			  p = &buffer[i] + 7;
			  break;
		  }
	  }
  }
  if (p == buffer)
	  return false; /// not found section

  return true;
}
