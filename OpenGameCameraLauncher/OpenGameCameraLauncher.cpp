// OpenGameCameraLauncher.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tlhelp32.h>

#include <filesystem>
#include <iostream>
#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include "GLFW/glfw3.h"
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

/*
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
*/
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // https://www.glfw.org/docs/3.3/window_guide.html#window_hints
    // https://github.com/ocornut/imgui/blob/master/docs/FONTS.txt
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    //glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(1, 1, "OpenGameCamera Launcher", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    bool show_another_window = true;
    ImVec4 clear_color = ImVec4(0, 0, 0, 1.00f);

    while (!glfwWindowShouldClose(window) || show_another_window)
    {
        glfwPollEvents();

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));
        //ImGui::SetNextWindowSize(ImVec2(600.0, 200.0));
        ImGui::Begin("OpenGameCamera Launcher", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        ImGui::End();

        ImGui::Render();
        //int display_w, display_h;
        //glfwGetFramebufferSize(window, &display_w, &display_h);
        //glViewport(0, 0, display_w, display_h);

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}