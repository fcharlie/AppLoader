///
////
///
///
///

#include <AppLoaderFile.h>

class BufferLine {
public:
private:
  wchar_t buffer[4096];
};

bool AppLoaderFile::Parse() { return true; }

bool AppLoaderFile::FasterParse(wchar_t *receive, uint32_t *counts) {
  if (metafile == nullptr) {
    return false;
  }
  HANDLE hFile = CreateFileW(metafile, GENERIC_READ, FILE_SHARE_READ, NULL,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == NULL)
    return false;
  DWORD bytes;
  uint8_t mark[8] = {0};
  if (!ReadFile(hFile, mark, 2, &bytes, NULL))
    return false;
  if (mark[0] != 0xFF || mark[1] != 0xEF)
    return false;
  // ReadFile(hFile,receive,2,)
  return true;
}