// OpenGameCameraLauncher.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tlhelp32.h>

#include <filesystem>
#include <iostream>
#include <string>

#include "resource.h"
#include "version.h"

#define ADD_QUOTES_HELPER(s) #s
#define ADD_QUOTES(s) ADD_QUOTES_HELPER(s)

#define TEMP_DIRECTORY_NAME  "OpenGameCamera"
#define TEMP_LIBRARY_NAME    "OpenGameCamera.dll"

#define TARGET_SWBF2_EXE     L"starwarsbattlefrontii.exe"

namespace fs = std::filesystem;

bool ExtractDLL(const HINSTANCE hInstance, std::wstring& path)
{
    bool success = false;

    HRSRC hResource = nullptr;
    HGLOBAL hFileResource = nullptr;
    LPVOID lpFile = nullptr;
    DWORD dwSize = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hFileMap = nullptr;
    LPVOID lpAddress = nullptr;

    hResource = FindResource(hInstance, MAKEINTRESOURCE(IDR_DLL1), L"DLL");
    if (hResource == nullptr) {
        goto Exit;
    }

    hFileResource = LoadResource(hInstance, hResource);
    if (hFileResource == nullptr) {
        goto Exit;
    }
    
    lpFile = LockResource(hFileResource);
    if (lpFile == nullptr) {
        goto Exit;
    }

    dwSize = SizeofResource(hInstance, hResource);
    if (dwSize == 0) {
        goto Exit;
    }

    hFile = CreateFile(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        goto Exit;
    }

    hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, dwSize, NULL);
    if (hFileMap == nullptr) {
        goto Exit;
    }

    lpAddress = MapViewOfFile(hFileMap, FILE_MAP_WRITE, 0, 0, 0);
    if (lpAddress == nullptr) {
        CloseHandle(hFileMap);
        CloseHandle(hFile);
        return false;
    }
    
    CopyMemory(lpAddress, lpFile, dwSize);
    
    success = true;

Exit:
    if (lpAddress) {
        UnmapViewOfFile(lpAddress);
    }

    if (hFileMap) {
        CloseHandle(hFileMap);
    }

    if (hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
    }

    return success;
}

bool InjectDllIntoSWBF2(std::wstring& dll_path)
{
    bool success = false;

    HANDLE hSnapshot = INVALID_HANDLE_VALUE;
    HANDLE hProcess = nullptr;
    PROCESSENTRY32 pe = { 0 };
    DWORD dwTargetPid = 0;
    LPBYTE pDllPath = nullptr;
    DWORD dwDllPathSize = 0;
    SIZE_T dwWrittenSize = 0;

    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        goto Exit;
    }

    pe.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe)) {
        goto Exit;
    }

    do {
        std::wstring process_exe(pe.szExeFile);
        std::transform(process_exe.begin(), process_exe.end(), process_exe.begin(), towlower);

        std::wcout << "pid=" << pe.th32ProcessID << "  name=" << process_exe << std::endl;

        if (process_exe.compare(TARGET_SWBF2_EXE) == 0) {
            dwTargetPid = pe.th32ProcessID;
            break;
        }
    } while (Process32Next(hSnapshot, &pe));

    if (dwTargetPid == 0) {
        goto Exit;
    }

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwTargetPid);
    if (hProcess == nullptr) {
        goto Exit;
    }

    dwDllPathSize = (dll_path.size() + 1) * sizeof(WCHAR);
    pDllPath = (LPBYTE) VirtualAllocEx(hProcess, nullptr, dwDllPathSize, MEM_COMMIT, PAGE_READWRITE);
    if (pDllPath == nullptr) {
        goto Exit;
    }

    if (!WriteProcessMemory(hProcess, pDllPath, dll_path.c_str(), dwDllPathSize, &dwWrittenSize)) {
        goto Exit;
    }

    // TODO: Get LoadLibraryW address
    // TODO: CreateRemoteThread

    success = true;

Exit:
    if (hProcess) {
        CloseHandle(hProcess);
    }

    if (hSnapshot != INVALID_HANDLE_VALUE) {
        CloseHandle(hSnapshot);
    }

    return success;
}

int main()
{
    bool force_overwrite = false;
    fs::path tmp_dir = fs::temp_directory_path() / TEMP_DIRECTORY_NAME / ADD_QUOTES(OGC_VERSION);
    if (!fs::exists(tmp_dir) && !fs::create_directories(tmp_dir)) {
        // TODO: User alert
        return 1;
    }

    fs::path dll_target = tmp_dir / TEMP_LIBRARY_NAME;
    std::wstring dll_path = dll_target.wstring();

    if (!fs::exists(dll_target) || force_overwrite) {
        if (!ExtractDLL(nullptr, dll_path)) {
            // TODO: User alert
            return 1;
        }
    } else {

    }

    InjectDllIntoSWBF2(dll_path);
}
