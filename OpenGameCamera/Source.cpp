#include <Windows.h>
#include <cstdint>
#include <iostream>
#include "Candy.hpp"
#include "renderer.h"
#include "customdraw.h"
#include "MouseManager.hpp"

// make Visual Studio shut up
// I'm warning you, stay back compiler!
#pragma warning(disable: 4996)

// global static variables
void* g_RenderView = NULL; // stores the RenderView pointer from GameRenderer
GlobalPostProcessSettings* g_PostProcess = nullptr;
bool g_origSSRValueSet = false;
bool g_origSSREnable = false;
bool g_origSSRFullResEnable = false;

// Camera Update function
__int64 __fastcall hkupdateCamera2(CameraObject* a1, CameraObject* a2)
{
	// NOTE(cstdr1): We only change the camera position if the source CameraObject is RenderView from GameRenderer!
	if (a2 == g_RenderView) {
		// if the camera position hasn't been set,
		if (Globals::g_CameraPosition.x == 0) {
			// set it to the current camera transform
			Globals::g_CameraPosition = a2->cameraTransform.o;
		}
		// if we're meant to be in frecam mode
		if (Settings::enableFreeCam) {
			// set the origin vector to our global vec4 override
			a2->cameraTransform.o = Globals::g_CameraPosition;
		}
	}

	return oupdateCamera2(a1, a2);
}

// keyboard hook function
bool __fastcall hkkeyboardUpdate(__int64 a1, unsigned __int8 a2, __int64 a3)
{
	// if the freecam is enabled, and it's set to freeze the player, return NULL
	if (Settings::enableFreeCam && Settings::freezePlayer) return 0;
	// otherwise return the original function
	return okeyboardUpdate(a1, a2, a3);
}

typedef __int64(*__fastcall tglobalPostProcessSub)(void*, __int64, GlobalPostProcessSettings*);
tglobalPostProcessSub oglobalPostProcessSub;

// Hook function prototype (globalPostProcessSub)
__int64 __fastcall hkglobalPostProcessSub(void* a1, __int64 a2, GlobalPostProcessSettings* a3)
{
	if (g_PostProcess == nullptr) std::cout << "g_PostProcess:\t0x" << std::hex << a3 << std::endl;
	g_PostProcess = a3;
	return oglobalPostProcessSub(a1, a2, a3);
}

// This gets called in the drawLoop of CustomDraw, so you can use ImGui functions here to display stuff in the top-left window.
void drawLoop() {
	if (g_PostProcess != nullptr && !g_origSSRValueSet) {
		// NOTE(cstdr1): Store original values for ssrEnable and ssrFullResEnable
		g_origSSREnable = g_PostProcess->screenSpaceRaytraceEnable;
		g_origSSRValueSet = true;
	}

	if (Settings::informationMenu && Globals::g_CameraPosition.x != NULL) {
		ImGui::Text("Information Menu");
		ImGui::Text("Camera XYZ: %f / %f / %f", Globals::g_CameraPosition.x, Globals::g_CameraPosition.y, Globals::g_CameraPosition.z);
	}

	// if the mouse hook hasn't been activated yet, display a help message
	if (Settings::informationMenu && MouseManager::arg1 == NULL) {
		ImGui::Text("Pause the game to initialize mouse hook");
	}

	// if the freecam is active and so is the menu, display this help message
	if (Settings::informationMenu && Settings::homeMenu) {
		ImGui::Text("Hide the menu to move the camera");
	}

	if (Settings::updateMouseState) {
		MouseManager::SetMouseState(Settings::homeMenu);
		Settings::updateMouseState = false;
	}

	// Toggle resolution scale
	if (Globals::ReadKeyOnce(Keys::enableResScale)) {
		Settings::enableResScale = !Settings::enableResScale;
	}

	// Set resolution scale
	if (Settings::enableResScale) {
		GameRenderer::GetInstance()->gameRenderSettings->resolutionScale = Settings::resScale;
	}
	else {
		GameRenderer::GetInstance()->gameRenderSettings->resolutionScale = 1.f;
	}

	// Set our FOV
	if (Settings::enableFreeCam) {
		float amount = .25f;
		if (Globals::ReadKey(Keys::speedUpCamera)) amount = 1.f;
		if (Globals::ReadKey(Keys::slowDownCamera)) amount = .05f;
		if (Globals::ReadKeyOnce(Keys::fovIncrease, 25)) {
			Settings::fov += amount;
		}
		if (Globals::ReadKeyOnce(Keys::fovDecrease, 25)) {
			Settings::fov -= amount;
		}
		GameRenderer::GetInstance()->gameRenderSettings->forceFov = Settings::fov;
	}
	else {
		GameRenderer::GetInstance()->gameRenderSettings->forceFov = -1;
	}

	// freeze time hotkey toggle
	if (Globals::ReadKeyOnce(Keys::freezeTime)) {
		Settings::freezeTime = !Settings::freezeTime;
	}

	// read enableFreeCam hotkey
	if (Globals::ReadKeyOnce(Keys::enableFreeCam)) {
		Settings::enableFreeCam = !Settings::enableFreeCam;
	}
	// read freezePlayer hotkey
	if (Globals::ReadKeyOnce(Keys::freezePlayer)) {
		Settings::freezePlayer = !Settings::freezePlayer;
	}

	// read disableUi hotkey
	if (Globals::ReadKeyOnce(Keys::disableUi)) {
		Settings::disableUi = !Settings::disableUi;
	}

	// read DoF hotkey
	if (Globals::ReadKeyOnce(Keys::enableDof)) {
		Settings::enableDof = !Settings::enableDof;
	}

	// SSR toggle
	if (g_PostProcess != nullptr) {
		g_PostProcess->screenSpaceRaytraceEnable = Settings::ssrEnable;
		g_PostProcess->screenSpaceRaytraceFullresEnable = Settings::ssrFullResEnable;
	}
	else if (g_PostProcess != nullptr) {
		g_PostProcess->screenSpaceRaytraceEnable = g_origSSREnable;
		g_PostProcess->screenSpaceRaytraceFullresEnable = g_origSSRFullResEnable;
	}
	
	// DoF toggle
	if (Settings::enableDof && g_PostProcess != nullptr) {
		g_PostProcess->spriteDofEnable = true;
		g_PostProcess->forceDofEnable = 1;
		g_PostProcess->forceSpriteDofBlurMax = Settings::dofBlurMax;
		g_PostProcess->forceSpriteDofFarStart = Settings::dofFarStart;
		g_PostProcess->forceSpriteDofFarEnd = Settings::dofFarEnd;
		g_PostProcess->forceSpriteDofNearStart = Settings::dofNearStart;
		g_PostProcess->forceSpriteDofNearEnd = Settings::dofNearEnd;
		g_PostProcess->enableForeground = Settings::dofEnableForeground;
		g_PostProcess->forceDofFocusDistance = Settings::focusDistance;
		g_PostProcess->spriteDofHalfResolutionEnable = Settings::spriteHalfResolution;
	}
	else if (g_PostProcess != nullptr) {
		g_PostProcess->spriteDofEnable = false;
	}


	// (dcat): testing bloom controls
	if (Settings::forceBloomEnable && g_PostProcess != nullptr) {
		g_PostProcess->bloomEnable = Settings::forceBloomEnable;
	}
	else if (g_PostProcess != nullptr) {
		g_PostProcess->bloomEnable = false;
	}

	// Exposure control
	if (Settings::enableFreeCam) {
		if (g_PostProcess != nullptr) {
			// Disabling this for now to fix white screen issue
			//g_PostProcess->forceEVEnable = true;
			//std::cout << std::hex << &g_PostProcess->forceEVEnable << std::endl;
		}
	}
	// Exposure
	if (Settings::evControl && g_PostProcess != nullptr) {
		g_PostProcess->forceEV = Settings::evControl;
	}

	// Should the time be frozen?
	GameTimeSettings::GetInstance()->timeScale = Settings::freezeTime ? 0.f : Settings::timeScale;

	// only process this stuff if the FreeCam is enabled
	if (Settings::enableFreeCam) {
		// disable the UI
		if (Settings::disableUi) UISettings::GetInstance()->drawEnable = false;

		// Get the GameRenderer pointer
		GameRenderer* pGameRenderer = GameRenderer::GetInstance();

		InputSettings::GetInstance()->mouseSensitivityPower = Settings::mouseSensitivity;

		// get the speed to move the camera at, and change it if the modifier keys are being pressed
		float amount = Settings::mainSpeed * Settings::camSens * 0.5;
		if (Globals::ReadKey(Keys::speedUpCamera)) amount = Settings::fastSpeed * Settings::camSens * 0.5;
		if (Globals::ReadKey(Keys::slowDownCamera)) amount = Settings::slowSpeed * Settings::camSens * 0.5;

		// set up some vectors
		Vec4 origin = pGameRenderer->renderView->transform.o;
		Vec4 xVec = pGameRenderer->renderView->transform.x;
		Vec4 yVec = pGameRenderer->renderView->transform.y;
		Vec4 zVec = pGameRenderer->renderView->transform.z;

		// modify the 'origin' vector based on what keys are being pressed
		if (Globals::ReadKey(Keys::cameraForward)) { // forwards
			origin = origin - zVec * amount;
		}
		if (Globals::ReadKey(Keys::cameraBack)) { // backwards
			origin = origin + zVec * amount;
		}
		if (Globals::ReadKey(Keys::cameraLeft)) {	// left
			origin = origin - xVec * amount;
		}
		if (Globals::ReadKey(Keys::cameraRight)) {	// right
			origin = origin + xVec * amount;
		}
		if (Globals::ReadKey(Keys::cameraDown)) { // down
			origin = origin - yVec * amount;
		}
		if (Globals::ReadKey(Keys::cameraUp)) { // up
			origin = origin + yVec * amount;
		}
		// Hacky fix for renderers to let the camera know it needs to update
		if (Globals::g_CameraPosition.w == -1) {
			Globals::g_CameraPosition.w = 0;
			origin = Globals::g_CameraPosition;
		}
		// set the global cameraPosition vec4 to our new location
		Globals::g_CameraPosition = origin;
	}
	else {
		// freecam is DISABLED
		if (Settings::disableUi) UISettings::GetInstance()->drawEnable = true;

		InputSettings::GetInstance()->mouseSensitivityPower = 1.f;
	}
}

DWORD __stdcall mainThread(HMODULE hOwnModule)
{
	// allocate a debug console
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	// output some scanned offsets
	std::cout << std::hex << "OFFSET_DXRENDERER:\t0x" << StaticOffsets::Get_OFFSET_DXRENDERER() << std::endl;
	std::cout << std::hex << "OFFSET_DEBUGRENDERER2:\t0x" << StaticOffsets::Get_OFFSET_DEBUGRENDERER2() << std::endl;
	std::cout << std::hex << "OFFSET_DRAWLINE:\t0x" << StaticOffsets::Get_OFFSET_DRAWLINE() << std::endl;
	std::cout << std::hex << "OFFSET_DRAWRECT2D:\t0x" << StaticOffsets::Get_OFFSET_DRAWRECT2D() << std::endl;
	std::cout << std::hex << "OFFSET_DRAWTEXT:\t0x" << StaticOffsets::Get_OFFSET_DRAWTEXT() << std::endl;
	std::cout << std::hex << "OFFSET_GAMERENDERER:\t0x" << StaticOffsets::Get_OFFSET_GAMERENDERER() << std::endl;
	std::cout << std::hex << "OFFSET_UISETTINGS:\t0x" << StaticOffsets::Get_OFFSET_UISETTINGS() << std::endl;
	std::cout << std::hex << "OFFSET_GAMETIMESETTINGS:\t0x" << StaticOffsets::Get_OFFSET_GAMETIMESETTINGS() << std::endl;
	std::cout << std::hex << "OFFSET_CLIENTGAMECONTEXT:\t0x" << StaticOffsets::Get_OFFSET_CLIENTGAMECONTEXT() << std::endl;
	std::cout << std::hex << "OFFSET_INPUTSETTINGS:\t0x" << StaticOffsets::Get_OFFSET_INPUTSETTINGS() << std::endl;
	std::cout << std::hex << "OFFSET_KEYBOARDUPDATE:\t0x" << StaticOffsets::Get_OFFSET_KEYBOARDUPDATE() << std::endl;
	std::cout << std::hex << "OFFSET_SETMOUSESTATE:\t0x" << StaticOffsets::Get_OFFSET_SETMOUSESTATE() << std::endl;
	std::cout << std::hex << "OFFSET_POSTPROCESSSUB:\t0x" << StaticOffsets::Get_OFFSET_POSTPROCESSSUB() << std::endl;
	std::cout << std::hex << "OFFSET_CAMERAHOOK2:\t0x" << StaticOffsets::Get_OFFSET_CAMERAHOOK2() << std::endl;

	// initialize the RenderView so we can correctly overwrite the camera location
	g_RenderView = GameRenderer::GetInstance()->renderView;

	// hook the function for setting our camera position manually
	// TODO(cstdr1): camerahook is still broken, investigating 
	Candy::CreateHook(StaticOffsets::Get_OFFSET_CAMERAHOOK2(), &hkupdateCamera2, &oupdateCamera2);

	// hook the function where keyboard input is processed
	Candy::CreateHook(StaticOffsets::Get_OFFSET_KEYBOARDUPDATE(), &hkkeyboardUpdate, &okeyboardUpdate);

	// hook the function where the mouse state is set
	Candy::CreateHook(StaticOffsets::Get_OFFSET_SETMOUSESTATE(), &MouseManager::hkSetMouseState, &MouseManager::oSetMouseState);

	// hook the function that access GlobalPostProcessSettings
	Candy::CreateHook(StaticOffsets::Get_OFFSET_POSTPROCESSSUB(), &hkglobalPostProcessSub, &oglobalPostProcessSub);

	// initialize our renderer, call the drawLoop function once per game frame
	Renderer* renderer = new Renderer();
	CustomDraw* draw = new CustomDraw();
	draw->Setup(drawLoop);

	// let this thread sit idling

	for (;;) {
		Sleep(100);
		if (GetAsyncKeyState(VK_END)) {
			// eject the mod
			printf("Fixing settings\n");
			// reset the timescale, and drawEnable
			GameTimeSettings::GetInstance()->timeScale = 1.f;
			UISettings::GetInstance()->drawEnable = true;
			InputSettings::GetInstance()->mouseSensitivityPower = 1.f;
			GameRenderer::GetInstance()->gameRenderSettings->forceFov = -1;
			if (g_PostProcess != nullptr) {
				g_PostProcess->spriteDofEnable = false;
				g_PostProcess->forceDofEnable = -1;
				g_PostProcess->screenSpaceRaytraceEnable = g_origSSREnable;
				g_PostProcess->screenSpaceRaytraceFullresEnable = g_origSSRFullResEnable;
			}
			printf("Unhooking\n");
			renderer->~Renderer();
			// now unhook everything we hooked

			//Renderer::shutdown();

			// TODO(cstdr1): camerahook is still broken, investigating 
			Candy::DestroyHook(StaticOffsets::Get_OFFSET_CAMERAHOOK2());
			Candy::DestroyHook(StaticOffsets::Get_OFFSET_KEYBOARDUPDATE());
			Candy::DestroyHook(StaticOffsets::Get_OFFSET_SETMOUSESTATE());
			Candy::DestroyHook(StaticOffsets::Get_OFFSET_POSTPROCESSSUB());
			// uninitialize minhook
			MH_Uninitialize();
			printf("Ejecting\n\nYou can close this window now\n");

			// free the console, then unload the module
			FreeConsole();
			FreeLibraryAndExitThread(hOwnModule, 0);
		}
	}

	return 0;
};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)mainThread, hModule, NULL, NULL);
	}
	return true;
}
