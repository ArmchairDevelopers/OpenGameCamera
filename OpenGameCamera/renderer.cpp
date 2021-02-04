#include "renderer.h"

// This is mostly from Polaris' rendering system, with some changes.

#include <windows.h>
#include "mainwindow.h"

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_dx11.h"
#include "imgui/examples/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"
#include "minhook/MinHook.h"

bool bLockFortInput;
std::list<Window*> Renderer::pUiInstances;

WNDPROC lpPrevWndFunc;
static HWND hWnd = 0;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
__declspec(dllexport) LRESULT CALLBACK WndProcHook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	if (Msg == WM_KEYUP && (wParam == Keys::showMenuKey.keyCode || (bLockFortInput && wParam == VK_ESCAPE))) {
		bLockFortInput = !bLockFortInput;

		ImGui::GetIO().MouseDrawCursor = bLockFortInput;
		gpRenderer->pMainWindow->SetEnabled(bLockFortInput);
		Settings::updateMouseState = true;
	}

	if (bLockFortInput)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);

		return TRUE;
	}

	return CallWindowProc(lpPrevWndFunc, hWnd, Msg, wParam, lParam);
}

HRESULT(*Present)(IDXGISwapChain* pInstance, UINT SyncInterval, UINT Flags) = nullptr;
__declspec(dllexport) HRESULT PresentHook(IDXGISwapChain* pInstance, UINT SyncInterval, UINT Flags)
{
	static float fWidth = 0;
	static float fHeight = 0;

	// Poll and handle messages (inputs, window resize, etc)
	static MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	if (::PeekMessage(&msg, hWnd, 0U, 0U, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	if (!gpRenderer->pCurrentDevice)
	{
		printf("Initializing Current Rendering Device.\n");
		pInstance->GetDevice(__uuidof(gpRenderer->pCurrentDevice), reinterpret_cast<PVOID*>(&gpRenderer->pCurrentDevice));
		gpRenderer->pCurrentDevice->GetImmediateContext(&gpRenderer->pCurrentContext);

		ID3D11Texture2D* pTarget = nullptr;
		pInstance->GetBuffer(0, __uuidof(pTarget), reinterpret_cast<PVOID*>(&pTarget));

		gpRenderer->pCurrentDevice->CreateRenderTargetView(pTarget, nullptr, &gpRenderer->pCurrentView);

		pTarget->Release();

		ID3D11Texture2D* pBuffer = nullptr;
		pInstance->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<PVOID*>(&pBuffer));

		D3D11_TEXTURE2D_DESC desc = { };
		pBuffer->GetDesc(&desc);

		if (!fWidth)
			lpPrevWndFunc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHook)));

		fWidth = static_cast<float>(desc.Width);
		fHeight = static_cast<float>(desc.Height);

		pBuffer->Release();

		if (!ImGui_ImplWin32_Init(hWnd))
		{
			MessageBox(0, "ImGui_ImplWIn32_Init Failed.", "Error", MB_ICONERROR);
			ExitProcess(EXIT_FAILURE);
		}
		if (!ImGui_ImplDX11_Init(gpRenderer->pCurrentDevice, gpRenderer->pCurrentContext))
		{
			MessageBox(0, "ImGui_ImplDX11_Init Failed.", "Error", MB_ICONERROR);
			ExitProcess(EXIT_FAILURE);
		}
		if (!ImGui_ImplDX11_CreateDeviceObjects())
		{
			MessageBox(0, "ImGui_ImplDX11_CreateDeviceObjects Failed.", "Error", MB_ICONERROR);
			ExitProcess(EXIT_FAILURE);
		}
	}

	ImGuiStyle& style = ImGui::GetStyle();

	ImVec4 col_text = ImColor(255, 255, 255);
	ImVec4 col_main = ImColor(235, 66, 66);
	ImVec4 col_back = ImColor(33, 36, 43);
	ImVec4 col_area = ImColor(51, 56, 69);

	style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(col_area.x, col_area.y, col_area.z, col_area.w + .1f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.45f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.85f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.90f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(col_main.x, col_main.y, col_main.z, 0.90f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.90f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.50f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.92f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	for (Window* ui : Renderer::pUiInstances)
	{
		if (ui->IsEnabled())
		{
			if (!bLockFortInput)
				ImGui::SetNextWindowBgAlpha(0.5f);
			ui->Draw();
		}
	}

	ImGui::SetNextWindowBgAlpha(1);

	ImGui::Render();
	gpRenderer->pCurrentContext->OMSetRenderTargets(1, &gpRenderer->pCurrentView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return Present(pInstance, SyncInterval, Flags);
}

// Resize hook.
HRESULT(*ResizeBuffers)(IDXGISwapChain* pInstance, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) = nullptr;
__declspec(dllexport) HRESULT ResizeBuffersHook(IDXGISwapChain* pInstance, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	for (Window* ui : Renderer::pUiInstances)
	{
		ui->Resize();
	}

	ImGui_ImplDX11_Shutdown();

	gpRenderer->pCurrentView->Release();
	gpRenderer->pCurrentContext->Release();
	gpRenderer->pCurrentDevice->Release();

	gpRenderer->pCurrentDevice = nullptr;

	return ResizeBuffers(pInstance, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

Renderer::Renderer()
{
	printf("Initializing Renderer\n");

	pMainWindow = new MainWindow;

	if (gpRenderer)
	{
		MessageBox(0, "Renderer is already initialized.", "Error", MB_ICONERROR);
		ExitProcess(EXIT_FAILURE);
	}

	gpRenderer = this;

	IDXGISwapChain* pSwapChain;
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pContext;

	auto featureLevel = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC desc = { };

	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = 1;

	printf("Attempting to find Battlefront window\n");

	hWnd = FindWindow("Frostbite", "STAR WARS Battlefront II");

	if (!hWnd)
	{
		MessageBox(0, "Failed to find Battlefront Window.", "Error", MB_ICONERROR);
		ExitProcess(EXIT_FAILURE);
	}

	desc.OutputWindow = hWnd;
	desc.Windowed = true;

	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, 0, &featureLevel, 1, D3D11_SDK_VERSION, &desc, &pSwapChain, &pDevice, nullptr, &pContext)))
	{
		MessageBox(0, "Failed to create DirectX 11 device.", "Error", MB_ICONERROR);
		ExitProcess(EXIT_FAILURE);
	}

	auto pTable = *reinterpret_cast<PVOID**>(pSwapChain);

	auto pPresent = pTable[8];
	auto pResizeBuffers = pTable[13];

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	pSwapChain->Release();
	pDevice->Release();
	pContext->Release();

	MH_CreateHook(pPresent, PresentHook, reinterpret_cast<PVOID*>(&Present));
	MH_EnableHook(pPresent);

	MH_CreateHook(pResizeBuffers, ResizeBuffersHook, reinterpret_cast<PVOID*>(&ResizeBuffers));
	MH_EnableHook(pResizeBuffers);
}

Renderer::~Renderer()
{
	gpRenderer = nullptr;

	auto pPresent = reinterpret_cast<PVOID*>(&Present);

	MH_DisableHook(pPresent);
	MH_RemoveHook(pPresent);

	auto pResizeBuffers = reinterpret_cast<PVOID*>(&ResizeBuffers);

	MH_DisableHook(pResizeBuffers);
	MH_RemoveHook(pResizeBuffers);
}