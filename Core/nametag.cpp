#include "XOR.hpp"
#include "core.hpp"
#include <string>
#include <thread>
#include <vector>
#include <windows.h>


namespace ArrayOfBytes {
constexpr BYTE NameTag[] = {0xC6, 0x01, 0x00, 0x8D, 0x49, 0x08,
                            0x83, 0xE8, 0x01, 0x75, 0xF5};
}

constexpr UINT NAMETAG_PATTERN_LENGTH = 11;
constexpr UINT NAMETAG_PATCH_OFFSET = 2;

static DWORD g_NameTagESPAddress = 0;
static bool g_NameTagESPFound = false;

static bool FindNameTagExternal(HANDLE hProcess, DWORD *outAddress) {
  DWORD currentAddress = 0x400000;
  DWORD maxAddress = 0x7FFFFFFF;
  MEMORY_BASIC_INFORMATION mbi;

  while (currentAddress < maxAddress) {
    if (!VirtualQueryEx(hProcess, (LPCVOID)currentAddress, &mbi, sizeof(mbi)))
      break;

    if (mbi.State == MEM_COMMIT &&
        (mbi.Protect == PAGE_READONLY || mbi.Protect == PAGE_READWRITE ||
         mbi.Protect == PAGE_EXECUTE_READ ||
         mbi.Protect == PAGE_EXECUTE_READWRITE)) {

      if (mbi.BaseAddress != nullptr &&
          mbi.RegionSize >= NAMETAG_PATTERN_LENGTH) {
        try {
          std::vector<BYTE> buffer(mbi.RegionSize);
          SIZE_T bytesRead = 0;

          if (ReadProcessMemory(hProcess, mbi.BaseAddress, buffer.data(),
                                mbi.RegionSize, &bytesRead)) {
            if (bytesRead >= NAMETAG_PATTERN_LENGTH) {
              SIZE_T max_search = bytesRead - NAMETAG_PATTERN_LENGTH;

              for (SIZE_T i = 0; i <= max_search; i++) {
                if (i + NAMETAG_PATTERN_LENGTH > mbi.RegionSize)
                  break;

                bool match = true;
                for (UINT j = 0; j < NAMETAG_PATTERN_LENGTH; j++) {
                  if (buffer[i + j] != ArrayOfBytes::NameTag[j]) {
                    match = false;
                    break;
                  }
                }

                if (match) {
                  *outAddress = (DWORD)((ULONG_PTR)mbi.BaseAddress + i);
                  return true;
                }
              }
            }
          }
        } catch (const std::bad_alloc &) {
          // Ignore memory allocation failure for this chunk and continue
        }
      }
    }
    if (mbi.RegionSize == 0)
      break;
    if (currentAddress + mbi.RegionSize < currentAddress)
      break;
    currentAddress += mbi.RegionSize;
  }
  return false;
}

void ToggleNameTagESP(bool enable) {
  if (pbProcessId == 0)
    return;

  if (isScanningNameTag.exchange(true)) {
    return; // Already scanning
  }

  std::thread([enable]() {
    HANDLE hProcess =
        OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
                        PROCESS_QUERY_INFORMATION,
                    FALSE, pbProcessId);
    if (!hProcess) {
      isScanningNameTag = false;
      return;
    }

    if (!g_NameTagESPFound) {
      if (!FindNameTagExternal(hProcess, &g_NameTagESPAddress)) {
        CloseHandle(hProcess);
        isScanningNameTag = false;
        return;
      }
      g_NameTagESPFound = true;
    }

    BYTE patchByte = enable ? 0x01 : 0x00;
    DWORD patchAddr = g_NameTagESPAddress + NAMETAG_PATCH_OFFSET;
    DWORD oldProtect;

    if (VirtualProtectEx(hProcess, (LPVOID)patchAddr, 1, PAGE_EXECUTE_READWRITE,
                         &oldProtect)) {
      WriteProcessMemory(hProcess, (LPVOID)patchAddr, &patchByte, 1, NULL);
      VirtualProtectEx(hProcess, (LPVOID)patchAddr, 1, oldProtect, &oldProtect);
    }

    CloseHandle(hProcess);
    isScanningNameTag = false;
  }).detach();
}