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
        std::cout << "[-] Failed to find resource: " << GetLastError() << std::endl;
        goto Exit;
    }

    hFileResource = LoadResource(hInstance, hResource);
    if (hFileResource == nullptr) {
        std::cout << "[-] Failed to load resource: " << GetLastError() << std::endl;
        goto Exit;
    }
    
    lpFile = LockResource(hFileResource);
    if (lpFile == nullptr) {
        std::cout << "[-] Failed to lock resource: " << GetLastError() << std::endl;
        goto Exit;
    }

    dwSize = SizeofResource(hInstance, hResource);
    if (dwSize == 0) {
        std::cout << "[-] Failed to get size of resource: " << GetLastError() << std::endl;
        goto Exit;
    }

    hFile = CreateFile(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cout << "[-] Failed to create file: " << GetLastError() << std::endl;
        goto Exit;
    }

    hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, dwSize, NULL);
    if (hFileMap == nullptr) {
        std::cout << "[-] Failed to create file mapping: " << GetLastError() << std::endl;
        goto Exit;
    }

    lpAddress = MapViewOfFile(hFileMap, FILE_MAP_WRITE, 0, 0, 0);
    if (lpAddress == nullptr) {
        std::cout << "[-] Failed to map view of file: " << GetLastError() << std::endl;
        goto Exit;
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
    HMODULE kernel32 = nullptr;
    FARPROC lpLoadLibraryW = nullptr;
    HANDLE hRemoteThread = nullptr;
    DWORD dwThreadId = 0;

    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cout << "[-] Failed to create toolhelp snapshot: " << GetLastError() << std::endl;
        goto Exit;
    }

    pe.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe)) {
        std::cout << "[-] Failed to get first process entry: " << GetLastError() << std::endl;
        goto Exit;
    }

    do {
        std::wstring process_exe(pe.szExeFile);
        std::transform(process_exe.begin(), process_exe.end(), process_exe.begin(), towlower);

        if (process_exe.compare(TARGET_SWBF2_EXE) == 0) {
            dwTargetPid = pe.th32ProcessID;
            break;
        }
    } while (Process32Next(hSnapshot, &pe));

    if (dwTargetPid == 0) {
        std::cout << "[-] Failed to find Star Wars Battlefront II process... are you sure it's running?" << std::endl;
        goto Exit;
    }

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwTargetPid);
    if (hProcess == nullptr) {
        std::cout << "[-] Failed to open process: " << GetLastError() << std::endl;
        goto Exit;
    }

    dwDllPathSize = (dll_path.size() + 1) * sizeof(WCHAR);
    pDllPath = (LPBYTE) VirtualAllocEx(hProcess, nullptr, dwDllPathSize, MEM_COMMIT, PAGE_READWRITE);
    if (pDllPath == nullptr) {
        std::cout << "[-] Failed to allocate memory: " << GetLastError() << std::endl;
        goto Exit;
    }

    if (!WriteProcessMemory(hProcess, pDllPath, dll_path.c_str(), dwDllPathSize, &dwWrittenSize)) {
        std::cout << "[-] Failed to write process memory: " << GetLastError() << std::endl;
        goto Exit;
    }

    kernel32 = GetModuleHandle(L"kernel32");
    if (kernel32 == nullptr) {
        std::cout << "[-] Failed to find kernel32: " << GetLastError() << std::endl;
        goto Exit;
    }

    lpLoadLibraryW = GetProcAddress(kernel32, "LoadLibraryW");
    if (lpLoadLibraryW == nullptr) {
        std::cout << "[-] Failed to get address of LoadLibraryW: " << GetLastError() << std::endl;
        goto Exit;
    }
    

    hRemoteThread = CreateRemoteThread(hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)lpLoadLibraryW, (LPVOID)pDllPath, 0, &dwThreadId);
    if (hRemoteThread == nullptr) {
        std::cout << "[-] Failed to create remote thread: " << GetLastError() << std::endl;
        goto Exit;
    }

    CloseHandle(hRemoteThread);

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

int error_exit()
{
    std::cout << std::endl;
    std::cout << "For troubleshooting help, please copy and paste the contents of this window to either the OpenGameCamera GitHub "
                 "or the Discord channel." << std::endl;
    std::cout << "Press [ENTER] to exit...";

    getchar();
    return 1;
}

int main()
{
#ifdef DEBUG
    bool force_overwrite = true;
#else
    bool force_overwrite = false;
#endif

    std::cout << "OpenGameCamera Launcher " << ADD_QUOTES(OGC_VERSION) << std::endl << std::endl;

    fs::path tmp_dir = fs::temp_directory_path() / TEMP_DIRECTORY_NAME / ADD_QUOTES(OGC_VERSION);
    
    std::cout << "[+] Setting up temporary directories to store OpenGameCamera.dll" << std::endl;
    if (!fs::exists(tmp_dir) && !fs::create_directories(tmp_dir)) {
        std::wcout << "[-] Failed to extract OpenGameCamera.dll!" << std::endl;
        return error_exit();
    }

    fs::path dll_target = tmp_dir / TEMP_LIBRARY_NAME;
    std::wstring dll_path = dll_target.wstring();

    if (!fs::exists(dll_target) || force_overwrite) {
        std::wcout << L"[+] Extracting OpenGameCamera.dll into " << tmp_dir << std::endl;

        if (!ExtractDLL(nullptr, dll_path)) {
            std::cout << "[-] Failed to inject OpenGameCamera into Star Wars Battlefront II" << std::endl;
            return error_exit();
        }
    } else {
        std::cout << "[+] Found and using cached version of OpenGameCamera.dll" << std::endl;
    }

    std::wcout << L"[+] OpenGameCamera extracted to " << dll_target << std::endl;
    std::cout << "[+] Injecting OpenGameCamera into Star Wars Battlefront II" << std::endl;

    if (!InjectDllIntoSWBF2(dll_path)) {
        std::cout << "[-] Failed to inject OpenGameCamera into Star Wars Battlefront II!" << std::endl;
        return error_exit();
    }

    std::cout << "[+] Done" << std::endl;
    return 0;
}
