#include <Windows.h>
#include <cstdint>
#include <iostream>
#include "Menu.hpp"
#include "MouseManager.hpp"

// make Visual Studio shut up
#pragma warning(disable: 4996)


// config data.  Change if you want different defaults
namespace Settings {
	bool enableFreeCam = false;
	bool freezePlayer = true;
	bool disableUi = true;
	bool freezeTime = false;
	float timeScale = 1.f;
	float mainSpeed = 0.1f;
	float slowSpeed = .001f;
	float fastSpeed = 2.f;
	float fov = 55;
	bool dofMenu = true;
	float mouseSensativity = 1.3f;
	// DOF Settings
	bool enableDof = false;
	float focusDistance = 0.f;
	bool spriteHalfResolution = false;
	float dofBlurMax = 3.f;
	float dofFarStart = 3.f;
	float dofFarEnd = 16.f;
	bool dofEnableForeground = false;
	float dofNearStart = 0.f;
	float dofNearEnd = 3.f;
}

// global static variables
void* g_RenderView = NULL; // stores the RenderView pointer from GameRenderer
Vec4 g_CameraPosition = { 0, 0, 0, 0 };
bool g_ShowMenu = true;
GlobalPostProcessSettings* g_PostProcess = nullptr;




// build our menu system
void buildMainMenu(Menu &menu) {
	// change the header text
	menu.header.text = "OpenGameCamera";

	// create a checkbox for showing/hiding the menu
	Element elem1;
	elem1.text = "Show Menu [" + Keys::showMenuKey.name + "]";
	elem1.type = Element::ElementType::checkBox;
	elem1.value = &g_ShowMenu;
	elem1.style.foregroundColor = defaultStyle.accentColor;

	// checkbox for enabling FreeCam
	Element elem2;
	elem2.text = "Enable FreeCam [" + Keys::enableFreeCam.name + "]";
	elem2.type = Element::ElementType::checkBox;
	elem2.value = &Settings::enableFreeCam;

	// checkbox for freezing the player when camera is enabled
	Element elem3;
	elem3.text = "Freeze Player [" + Keys::freezePlayer.name + "]";
	elem3.value = &Settings::freezePlayer;
	elem3.type = Element::ElementType::checkBox;

	// checkbox for disabling the UI when the camera is enabled
	Element elemDisableUi;
	elemDisableUi.text = "Disable UI [" + Keys::disableUi.name + "]";
	elemDisableUi.value = &Settings::disableUi;
	elemDisableUi.type = Element::ElementType::checkBox;

	// checkbox for freezing time 
	Element elemFreeze;
	elemFreeze.text = "Freeze Time [" + Keys::freezeTime.name + "]";
	elemFreeze.type = Element::ElementType::checkBox;
	elemFreeze.value = &Settings::freezeTime;


	Element elemFovP;
	elemFovP.text = "Increase FOV [" + Keys::fovIncrease.name + "]";

	Element elemFovM;
	elemFovM.text = "Decrease FOV [" + Keys::fovDecrease.name + "]";

	Element elemMouseSens;
	elemMouseSens.text = "Mouse sensitivity";
	elemMouseSens.type = Element::ElementType::floatSlider;
	elemMouseSens.value = &Settings::mouseSensativity;
	elemMouseSens.min = 0.1;
	elemMouseSens.max = 1000;
	elemMouseSens.step = .5;

	Element elemTimeScale;
	elemTimeScale.text = "Time scale";
	elemTimeScale.type = Element::ElementType::floatSlider;
	elemTimeScale.value = &Settings::timeScale;
	elemTimeScale.min = 0.1;
	elemTimeScale.max = 1.f;
	elemTimeScale.step = 0.1;

	Element elemShowDofMenu;
	elemShowDofMenu.text = "Show DOF Menu";
	elemShowDofMenu.type = Element::ElementType::checkBox;
	elemShowDofMenu.value = &Settings::dofMenu;


	// add them to the menu
	menu.elements.push_back(elem1);
	menu.elements.push_back(elem2);
	menu.elements.push_back(elem3);
	menu.elements.push_back(elemDisableUi);
	menu.elements.push_back(elemFreeze);
	menu.elements.push_back(elemFovP);
	menu.elements.push_back(elemFovM);
	menu.elements.push_back(elemMouseSens);
	menu.elements.push_back(elemTimeScale);
	menu.elements.push_back(elemShowDofMenu);
}

void buildDofMenu(Menu& menu) {
	menu.header.text = "DOF";
	menu.absolutePos.x += 600;

	Element elemDofEnable;
	elemDofEnable.text = "Enable DOF effects";
	elemDofEnable.type = Element::ElementType::checkBox;
	elemDofEnable.value = &Settings::enableDof;

	Element elmDofEnableFg;
	elmDofEnableFg.text = "DOF Enable Foreground";
	elmDofEnableFg.type = Element::ElementType::checkBox;
	elmDofEnableFg.value = &Settings::dofEnableForeground;

	Element elemDofBlur;
	elemDofBlur.text = "DOF Max Blur";
	elemDofBlur.type = Element::ElementType::floatSlider;
	elemDofBlur.value = &Settings::dofBlurMax;
	elemDofBlur.min = 0;
	elemDofBlur.max = 50;
	elemDofBlur.step = .25;

	Element elemFocusDist;
	elemFocusDist.text = "DOF Focus Distance";
	elemFocusDist.type = Element::ElementType::floatSlider;
	elemFocusDist.value = &Settings::focusDistance;
	elemFocusDist.min = 0;
	elemFocusDist.max = 10000;
	elemFocusDist.step = .5;


	Element elemDofFarStart;
	elemDofFarStart.text = "DOF FAR Start";
	elemDofFarStart.type = Element::ElementType::floatSlider;
	elemDofFarStart.value = &Settings::dofFarStart;
	elemDofFarStart.min = 0;
	elemDofFarStart.max = 10000;
	elemDofFarStart.step = .5f;

	Element elemDofFarEnd;
	elemDofFarEnd.text = "DOF FAR End";
	elemDofFarEnd.type = Element::ElementType::floatSlider;
	elemDofFarEnd.value = &Settings::dofFarEnd;
	elemDofFarEnd.min = 0;
	elemDofFarEnd.max = 10000;
	elemDofFarEnd.step = .5f;

	Element elemDofNearStart;
	elemDofNearStart.text = "DOF NEAR Start";
	elemDofNearStart.type = Element::ElementType::floatSlider;
	elemDofNearStart.value = &Settings::dofNearStart;
	elemDofNearStart.min = 0;
	elemDofNearStart.max = 10000;
	elemDofNearStart.step = .5f;

	Element elemDofNearEnd;
	elemDofNearEnd.text = "DOF NEAR End";
	elemDofNearEnd.type = Element::ElementType::floatSlider;
	elemDofNearEnd.value = &Settings::dofNearEnd;
	elemDofNearEnd.min = 0;
	elemDofNearEnd.max = 10000;
	elemDofNearEnd.step = .5f;

	Element elemDofHalfRes;
	elemDofHalfRes.text = "DOF Sprite Half-Resolution";
	elemDofHalfRes.type = Element::ElementType::checkBox;
	elemDofHalfRes.value = &Settings::spriteHalfResolution;

	menu.elements.push_back(elemDofEnable);
	menu.elements.push_back(elmDofEnableFg);
	menu.elements.push_back(elemDofBlur);
	menu.elements.push_back(elemFocusDist);
	menu.elements.push_back(elemDofFarStart);
	menu.elements.push_back(elemDofFarEnd);
	menu.elements.push_back(elemDofNearStart);
	menu.elements.push_back(elemDofNearEnd);
	menu.elements.push_back(elemDofHalfRes);
}

// Camera Update function
__int64 __fastcall hkupdateCamera2(CameraObject* a1, CameraObject* a2)
{
	// NOTE(cstdr1): We only change the camera position if the source CameraObject is RenderView from GameRenderer!
	if (a2 == g_RenderView) {
		// if the camera position hasn't been set,
		if (g_CameraPosition.x == 0) {
			// set it to the current camera transform
			g_CameraPosition = a2->cameraTransform.o;
		}
		// if we're meant to be in frecam mode
		if (Settings::enableFreeCam) {
			// set the origin vector to our global vec4 override
			a2->cameraTransform.o = g_CameraPosition;
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
	g_PostProcess = a3;
	return oglobalPostProcessSub(a1, a2, a3);
}

Menu mainMenu;
Menu dofMenu;

void drawLoop(Renderer* pRenderer, uint32_t width, uint32_t height) {

	// if the mouse hook hasn't been activated yet, display a help message
	if (MouseManager::arg1 == NULL) {
		pRenderer->drawText(20, 20, defaultStyle.accentColor, "Pause the game to initialize mouse hook", 2);
	}

	// if the freecam is active and so is the menu, display this help message
	if (g_ShowMenu) {
		pRenderer->drawText(20, 50, defaultStyle.foregroundColor, "Hide the menu to move the camera", 1.25f);
	}

	// if the global bool says we should draw the menu, do so
	if (g_ShowMenu) {
		mainMenu.draw(pRenderer);
		if (Settings::dofMenu) dofMenu.draw(pRenderer);
	}

	// Set our FOV
	if (Settings::enableFreeCam) {
		float amount = .25f;
		if (KeyMan::ReadKey(Keys::speedUpCamera)) amount = 1.f;
		if (KeyMan::ReadKey(Keys::slowDownCamera)) amount = .05f;
		if (KeyMan::ReadKeyOnce(Keys::fovIncrease, 25)) {
			Settings::fov += amount;
		}
		if (KeyMan::ReadKeyOnce(Keys::fovDecrease, 25)) {
			Settings::fov -= amount;
		}
		GameRenderer::GetInstance()->gameRenderSettings->forceFov = Settings::fov;
	}
	else {
		GameRenderer::GetInstance()->gameRenderSettings->forceFov = -1;
	}

	// freeze time hotkey toggle
	if (KeyMan::ReadKeyOnce(Keys::freezeTime)) {
		Settings::freezeTime = !Settings::freezeTime;
	}
	
	// toggle the menu button if they press the key
	if (KeyMan::ReadKeyOnce(Keys::showMenuKey)) {
		g_ShowMenu = !g_ShowMenu;
		MouseManager::SetMouseState(g_ShowMenu);
	}

	// read enableFreeCam hotkey
	if (KeyMan::ReadKeyOnce(Keys::enableFreeCam)) {
		Settings::enableFreeCam = !Settings::enableFreeCam;
	}
	// read freezePlayer hotkey
	if (KeyMan::ReadKeyOnce(Keys::freezePlayer)) {
		Settings::freezePlayer = !Settings::freezePlayer;
	}

	// read disableUi hotkey
	if (KeyMan::ReadKeyOnce(Keys::disableUi)) {
		Settings::disableUi = !Settings::disableUi;
	}

	if (Settings::enableDof) {
		if (g_PostProcess != nullptr) {
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
	}
	else {
		if (g_PostProcess != nullptr) {
			g_PostProcess->spriteDofEnable = false;
		}
	}

	// Should the time be frozen?
	GameTimeSettings::GetInstance()->timeScale = Settings::freezeTime ? 0.f : Settings::timeScale;

	// only process this stuff if the FreeCam is enabled
	if (Settings::enableFreeCam) {
		// disable the UI
		if (Settings::disableUi) UISettings::GetInstance()->drawEnable = false;

		// Get the GameRenderer pointer
		GameRenderer* pGameRenderer = GameRenderer::GetInstance();

		InputSettings::GetInstance()->mouseSensitivityPower = Settings::mouseSensativity;

		// get the speed to move the camera at, and change it if the modifier keys are being pressed
		float amount = Settings::mainSpeed;
		if (KeyMan::ReadKey(Keys::speedUpCamera)) amount = Settings::fastSpeed;
		if (KeyMan::ReadKey(Keys::slowDownCamera)) amount = Settings::slowSpeed;

		// set up some vectors
		Vec4 origin = pGameRenderer->renderView->transform.o;
		Vec4 xVec = pGameRenderer->renderView->transform.x;
		Vec4 yVec = pGameRenderer->renderView->transform.y;
		Vec4 zVec = pGameRenderer->renderView->transform.z;

		// modify the 'origin' vector based on what keys are being pressed
		if (KeyMan::ReadKey(Keys::cameraForward)) { // forwards
			origin = origin - zVec * amount;
		}
		if (KeyMan::ReadKey(Keys::cameraBack)) { // backwards
			origin = origin + zVec * amount;
		}
		if (KeyMan::ReadKey(Keys::cameraLeft)) {	// left
			origin = origin - xVec * amount;
		}
		if (KeyMan::ReadKey(Keys::cameraRight)) {	// right
			origin = origin + xVec * amount;
		}
		if (KeyMan::ReadKey(Keys::cameraDown)) { // down
			origin = origin - yVec * amount;
		}
		if (KeyMan::ReadKey(Keys::camerUp)) { // up
			origin = origin + yVec * amount;
		}
		// set the global cameraPosition vec4 to our new location
		g_CameraPosition = origin;
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
	std::cout << std::hex << "OFFSET_INPUTSETTINGS:\t0x" << StaticOffsets::Get_OFFSET_INPUTSETTINGS() << std::endl;
	std::cout << std::hex << "OFFSET_KEYBOARDUPDATE:\t0x" << StaticOffsets::Get_OFFSET_KEYBOARDUPDATE() << std::endl;
	std::cout << std::hex << "OFFSET_SETMOUSESTATE:\t0x" << StaticOffsets::Get_OFFSET_SETMOUSESTATE() << std::endl;
	std::cout << std::hex << "OFFSET_POSTPROCESSSUB:\t0x" << StaticOffsets::Get_OFFSET_POSTPROCESSSUB() << std::endl;
	std::cout << std::hex << "OFFSET_CAMERAHOOK2:\t0x" << StaticOffsets::Get_OFFSET_CAMERAHOOK2() << std::endl;

	// initialize the RenderView so we can correctly overwrite the camera location
	g_RenderView = GameRenderer::GetInstance()->renderView;

	// build our menu
	buildMainMenu(mainMenu);
	buildDofMenu(dofMenu);
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
	Renderer::setup(drawLoop);

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
			}
			printf("Unhooking\n");
			// now unhook everything we hooked
			Renderer::shutdown();
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
