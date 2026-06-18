#include "core.hpp"
#include <windows.h>
#include <TlHelp32.h>
#include "XOR.hpp"
#include <string>

namespace ArrayOfBytes
{
    constexpr BYTE NameTag[] = { 0xC6, 0x01, 0x00, 0x8D, 0x49, 0x08, 0x83, 0xE8, 0x01, 0x75, 0xF5 };
}

constexpr UINT NAMETAG_PATTERN_LENGTH = 11;
constexpr UINT NAMETAG_PATCH_OFFSET = 2;

static DWORD g_NameTagESPAddress = 0;
static bool g_NameTagESPFound = false;

static DWORD GetPBProcessId()
{
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32 pe = { 0 };
    pe.dwSize = sizeof(PROCESSENTRY32);
    DWORD pid = 0;

    std::string targetExe = XOR("PointBlank.exe");

    if (Process32First(hSnap, &pe))
    {
        do
        {
            if (_stricmp(pe.szExeFile, targetExe.c_str()) == 0)
            {
                pid = pe.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnap, &pe));
    }
    CloseHandle(hSnap);
    return pid;
}

static bool FindNameTagExternal(HANDLE hProcess, DWORD* outAddress)
{
    DWORD currentAddress = 0x400000;
    DWORD maxAddress = 0x7FFFFFFF;
    MEMORY_BASIC_INFORMATION mbi;

    while (currentAddress < maxAddress)
    {
        if (!VirtualQueryEx(hProcess, (LPCVOID)currentAddress, &mbi, sizeof(mbi))) break;

        if (mbi.State == MEM_COMMIT &&
            (mbi.Protect == PAGE_READONLY || mbi.Protect == PAGE_READWRITE ||
                mbi.Protect == PAGE_EXECUTE_READ || mbi.Protect == PAGE_EXECUTE_READWRITE))
        {

            if (mbi.BaseAddress != nullptr && mbi.RegionSize >= NAMETAG_PATTERN_LENGTH)
            {
                BYTE* buffer = new BYTE[mbi.RegionSize];
                SIZE_T bytesRead = 0;

                if (ReadProcessMemory(hProcess, mbi.BaseAddress, buffer, mbi.RegionSize, &bytesRead))
                {

                    if (bytesRead >= NAMETAG_PATTERN_LENGTH)
                    {
                        SIZE_T max_search = bytesRead - NAMETAG_PATTERN_LENGTH;

                        for (SIZE_T i = 0; i <= max_search; i++)
                        {
                            if (i + NAMETAG_PATTERN_LENGTH > mbi.RegionSize) break;

                            bool match = true;
                            for (UINT j = 0; j < NAMETAG_PATTERN_LENGTH; j++)
                            {
                                if (buffer[i + j] != ArrayOfBytes::NameTag[j])
                                {
                                    match = false;
                                    break;
                                }
                            }

                            if (match)
                            {
                                *outAddress = (DWORD)((ULONG_PTR)mbi.BaseAddress + i);
                                delete[] buffer;
                                return true;
                            }
                        }
                    }
                }
                delete[] buffer;
            }
        }
        if (mbi.RegionSize == 0) break;
        if (currentAddress + mbi.RegionSize < currentAddress) break;
        currentAddress += mbi.RegionSize;
    }
    return false;
}

void ToggleNameTagESP(bool enable)
{
    DWORD pid = GetPBProcessId();
    if (pid == 0) return;

    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!hProcess) return;

    if (!g_NameTagESPFound)
    {
        if (!FindNameTagExternal(hProcess, &g_NameTagESPAddress))
        {
            CloseHandle(hProcess);
            return;
        }
        g_NameTagESPFound = true;
    }

    BYTE patchByte = enable ? 0x01 : 0x00;
    DWORD patchAddr = g_NameTagESPAddress + NAMETAG_PATCH_OFFSET;
    DWORD oldProtect;

    if (VirtualProtectEx(hProcess, (LPVOID)patchAddr, 1, PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        WriteProcessMemory(hProcess, (LPVOID)patchAddr, &patchByte, 1, NULL);
        VirtualProtectEx(hProcess, (LPVOID)patchAddr, 1, oldProtect, &oldProtect);
    }

    CloseHandle(hProcess);
}