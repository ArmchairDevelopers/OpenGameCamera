#pragma once
#include "SigScan/StaticOffsets.h"
#include "BasicTypes.hpp"

// reverse engineered game classes

class GameRenderer;
class RenderView;
class UISettings;
class GameTimeSettings;
class CameraObject;
class GameRenderSettings;


class GameRenderer {
public:
	char pad_0000[1304]; //0x0000
	class GameRenderSettings* gameRenderSettings; //0x0510
	char pad_0520[24]; //0x0520
	class RenderView* renderView; //0x0538
	char pad_0540[4872]; //0x0540
	// static method to return the default instance
	static GameRenderer* GetInstance(void) {
		return *(GameRenderer**)StaticOffsets::Get_OFFSET_GAMERENDERER();
	}
};

class GameRenderSettings {
public:
	char pad[0x5c];
	float forceFov;
};

// RenderView structure, where we can read the camera transform
class RenderView {
public:
	Matrix4x4 transform;
};

class UISettings {
public:
	char pad_000[0x44];
	bool drawEnable;
	// static method to return the default instance, from an offset of GameTimeSettings's pointer
	static UISettings* GetInstance(void) {
		return *(UISettings**)(StaticOffsets::Get_OFFSET_GAMETIMESETTINGS() + 0x10);
	}
};

// used for freezing time
class GameTimeSettings {
public:
	char pad[0x38];
	float timeScale;
	static GameTimeSettings* GetInstance() {
		return *(GameTimeSettings**)StaticOffsets::Get_OFFSET_GAMETIMESETTINGS();
	}
};

// our CameraObject that needs to be used for the camera hook
class CameraObject {
public:
	Matrix4x4 cameraTransform;
};
