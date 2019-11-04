#include <Windows.h>
#include <cstdint>
#include <iostream>
#include "Menu.hpp"
#include "MouseManager.hpp"

// make Visual Studio shut up
#pragma warning(disable: 4996)

#define OFFSET_CAMERAHOOK2 0x140AC9D40
#define OFFSET_KEYBOARDUPDATE 0x1467a04a0
#define OFFSET_SETMOUSESTATE 0x1474DA990

// config data.  Change if you want different defaults
namespace Settings {
	bool enableFreeCam = false;
	bool freezePlayer = true;
	bool disableUi = true;
	bool freezeTime = false;
	float mainSpeed = 0.1f;
	float slowSpeed = .001f;
	float fastSpeed = 2.f;
}

// global static variables
Vec4 g_CameraPosition = { 0, 0, 0, 0 };
bool g_ShowMenu = true;


// build our menu system
void buildMenu(Menu &menu) {
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

	// add them to the menu
	menu.elements.push_back(elem1);
	menu.elements.push_back(elem2);
	menu.elements.push_back(elem3);
	menu.elements.push_back(elemDisableUi);
	menu.elements.push_back(elemFreeze);
}

// Camera Update function
__int64 __fastcall hkupdateCamera2(__int64 a1, CameraObject* a2)
{
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

Menu mainMenu;
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
	}

	// freeze time hotkey toggle
	if (KeyMan::ReadKeyOnce(Keys::freezeTime, 100)) {
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



	// Should the time be frozen?
	GameTimeSettings::GetInstance()->timeScale = Settings::freezeTime ? 0.f : 1.f;

	// only process this stuff if the FreeCam is enabled
	if (Settings::enableFreeCam) {
		// disable the UI
		if (Settings::disableUi) UISettings::GetInstance()->drawEnable = false;

		// Get the GameRenderer pointer
		GameRenderer* pGameRenderer = GameRenderer::GetInstance();

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
	std::cout << std::hex << "OFFSET_GAMETIMESETTINGS:\t0x" << StaticOffsets::Get_OFFSET_GAMETIMESETTINGS() << std::endl;
	
	// build our menu
	buildMenu(mainMenu);

	// hook the function for setting our camera position manually
	Candy::CreateHook(OFFSET_CAMERAHOOK2, &hkupdateCamera2, &oupdateCamera2);

	// hook the function where keyboard input is processed
	Candy::CreateHook(OFFSET_KEYBOARDUPDATE, &hkkeyboardUpdate, &okeyboardUpdate);

	// hook the function where the mouse state is set
	Candy::CreateHook(OFFSET_SETMOUSESTATE, &MouseManager::hkSetMouseState, &MouseManager::oSetMouseState);
	
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

			printf("Unhooking\n");
			// now unhook everything we hooked
			Renderer::shutdown();
			Candy::DestroyHook(OFFSET_CAMERAHOOK2);
			Candy::DestroyHook(OFFSET_KEYBOARDUPDATE);
			Candy::DestroyHook(OFFSET_SETMOUSESTATE);

			// uninitialize minhook
			MH_Uninitialize();
			printf("Ejecting\n\nYou can close this window now\n");

			// free the console, then unload the module
			FreeConsole();
			FreeLibraryAndExitThread(hOwnModule, 0);
		}
	}
};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)mainThread, hModule, NULL, NULL);
	}
	return true;
}