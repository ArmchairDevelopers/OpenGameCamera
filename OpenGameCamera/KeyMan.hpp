#pragma once
#include <Windows.h>
#include <ctime>
#include <string>
// keyboard manager

// a key definition. Has the name of the key, and it's keycode
struct KeyDef {
	std::string name;
	unsigned int keyCode;
};

// static key definitions, for preset keys we'll need
class Keys {
public:
	static KeyDef showMenuKey;
	static KeyDef enableFreeCam;
	static KeyDef freezePlayer;
	static KeyDef recordCameraTrack;
	static KeyDef speedUpCamera;
	static KeyDef slowDownCamera;
	static KeyDef cameraForward;
	static KeyDef cameraBack;
	static KeyDef cameraLeft;
	static KeyDef cameraRight;
	static KeyDef camerUp;
	static KeyDef cameraDown;
	static KeyDef disableUi;
	static KeyDef freezeTime;
	static KeyDef fovIncrease;
	static KeyDef fovDecrease;
	static KeyDef enableResScale;
};


// KeyMan class, for reading keystates
class KeyMan {
private:
	// the last time a key was clicked
	static time_t lastTime;

	// check if the window is active
	static bool isWindowActive();

	// function for getting the current game's window
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
public:
	// read key with a minimum time between keypresses
	static bool ReadKeyOnce(unsigned int keyCode, unsigned int timer = 250);
	static bool ReadKeyOnce(KeyDef keyDef, unsigned int timer = 250);

	// read a key without the time delay
	static bool ReadKey(unsigned int keyCode);
	static bool ReadKey(KeyDef keyDef);

	// require the game window to be in the foreground
	static bool requireActive;

	// handle to the game window
	static HWND windowHandle;
};