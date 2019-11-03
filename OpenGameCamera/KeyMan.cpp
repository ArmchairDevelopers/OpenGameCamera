#include "KeyMan.hpp"

// read a key with a minimum time required before it is read again
bool KeyMan::ReadKeyOnce(unsigned int keycode, unsigned int timer) {
	// if foreground is required, check if the window is in the foreground
	if (KeyMan::requireActive && !KeyMan::isWindowActive()) return false;

	// get the current time - the last keypress time AND
	// check if the key is being pressed
	if ((clock() - lastTime > timer) && GetAsyncKeyState(keycode)) {
		// set the last clock time, then return true
		KeyMan::lastTime = clock();
		return true;
	}
	// return false if the previous clock time wasn't within the minimum required, or the key wasn't pressed
	return false;
}

// call the above function
bool KeyMan::ReadKeyOnce(KeyDef keyDef, unsigned int timer) {
	return KeyMan::ReadKeyOnce(keyDef.keyCode, timer);
}

// read a key without giving a damn about how many times it was pressed, or if it's being held
bool KeyMan::ReadKey(unsigned int keycode) {
	// check the active window
	if (KeyMan::requireActive && !KeyMan::isWindowActive()) return false;

	// return the keystate
	return GetAsyncKeyState(keycode);
}

// call the above
bool KeyMan::ReadKey(KeyDef keyDef) {
	return KeyMan::ReadKey(keyDef.keyCode);
}

// return if the game window is in the foreground
bool KeyMan::isWindowActive() {
	// if the window handle is invalid, 
	if (KeyMan::windowHandle == NULL) {
		// call our EnumWindows function to get the window
		EnumWindows(KeyMan::EnumWindowsProc, NULL);
	}
	// then return a check if the active window is the game window
	return GetActiveWindow() == KeyMan::windowHandle;
}

// enumerate all windows
BOOL CALLBACK KeyMan::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	// check if it's our console window, if so skip
	if (hwnd == GetConsoleWindow()) return TRUE;

	// get the process ID of the window
	DWORD lpdwProcessId;
	GetWindowThreadProcessId(hwnd, &lpdwProcessId);

	// if the process ID is our own,
	if (lpdwProcessId == GetCurrentProcessId())
	{
		// set our static window handle and exit EnumWindow
		KeyMan::windowHandle = hwnd;
		return FALSE;
	}
	return TRUE;
}

// the current window handle
HWND KeyMan::windowHandle = NULL;

// require the foreground to be active
bool KeyMan::requireActive = false;

// the last clock
time_t KeyMan::lastTime = clock();

// key definitions for our preset keys
KeyDef Keys::showMenuKey = { "HOME", VK_HOME };
KeyDef Keys::enableFreeCam = { "F5", VK_F5 };
KeyDef Keys::freezePlayer = { "F6", VK_F6 };
KeyDef Keys::speedUpCamera = { "SHIFT", VK_SHIFT };
KeyDef Keys::slowDownCamera = { "ALT", VK_MENU };
KeyDef Keys::cameraForward = { "W", 0x57 };
KeyDef Keys::cameraBack = { "S", 0x53 };
KeyDef Keys::cameraLeft = { "A", 0x41 };
KeyDef Keys::cameraRight = { "D", 0x44 };
KeyDef Keys::camerUp = { "SPACE", VK_SPACE };
KeyDef Keys::cameraDown = { "CONTROL", VK_CONTROL };
KeyDef Keys::disableUi = { "F7", VK_F7 };
KeyDef Keys::freezeTime = { "F8", VK_F8 };