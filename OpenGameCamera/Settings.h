#pragma once
#ifndef SETTINGS_H
#define SETTINGS_H

// config data.  Change if you want different defaults
namespace Settings {
	bool enableFreeCam = false;
	bool disableUi = true;
	bool cameraMenu = true;
	bool dofMenu = true;
	bool effectsMenu = true;
	// Camera Settings
	float evControl = 5.f;
	float camSens = 1.f;
	float fov = 55;
	float resScale = 1.f;
	bool enableResScale = false;
	float mainSpeed = 0.1f;
	float slowSpeed = .01f;
	float fastSpeed = 1.f;
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
	// Effects Menu
	bool freezeTime = false;
	float timeScale = 1.f;
	bool freezePlayer = true;
	bool forceBloomEnable = true;
	bool ssrEnable = false;
	bool ssrFullResEnable = false;
}

#endif