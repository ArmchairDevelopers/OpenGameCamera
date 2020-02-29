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
class GlobalPostProcessSettings;
class InputSettings;

class GameRenderer {
public:
	char pad_0000[1304]; //0x0000
	class GameRenderSettings* gameRenderSettings; //0x0510
	char pad_0520[24]; //0x0520
	// NOTE(cstdr1): Below is the transform for showing the camera location
	class RenderView* renderView; //0x0538
	char pad_0540[4872]; //0x0540
	// static method to return the default instance
	static GameRenderer* GetInstance(void) {
		return *(GameRenderer**)StaticOffsets::Get_OFFSET_GAMERENDERER();
	}
};

class GameRenderSettings {
public:
	char pad_0000[40]; //0x0000
	float resolutionScale; //0x0028
	char pad_002C[48]; //0x002C
	float forceFov; //0x005C
	char pad_0060[1000]; //0x0060
};

class GlobalPostProcessSettings
{
public:
	char pad_0000[148]; //0x0000
	float forceEV; //0x0094
	char pad_0098[44]; //0x0098
	int32_t forceDofEnable; //0x00C4
	float forceDofBlurFactor; //0x00C8
	char pad_00CC[4]; //0x00CC
	float forceDofFocusDistance; //0x00D0
	char pad_00D4[20]; //0x00D4
	float forceSpriteDofNearStart; //0x00E8
	float forceSpriteDofNearEnd; //0x00EC
	float forceSpriteDofFarStart; //0x00F0
	float forceSpriteDofFarEnd; //0x00F4
	float forceSpriteDofBlurMax; //0x00F8
	char pad_00FC[284]; //0x00FC
	bool forceEVEnable; //0x0218
	char pad_0219[6]; //0x0219
	bool bloomEnable; //0x021F
	char pad_0220[21]; //0x0220
	bool spriteDofEnable; //0x0235
	char pad_0236[1]; //0x0236
	bool enableForeground; //0x0237
	char pad_0238[2]; //0x0238
	bool spriteDofHalfResolutionEnable; //0x023A
	char pad_023B[14]; //0x023B
	bool screenSpaceRaytraceEnable; //0x0249
	char pad_024A[3]; //0x024A
	bool screenSpaceRaytraceFullresEnable; //0x024D
	char pad_024E[3573]; //0x024E
}; //Size: 0x1043

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
		return *(UISettings**)(StaticOffsets::Get_OFFSET_UISETTINGS());
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

class InputSettings {
public:
	char pad[0x94];
	float mouseSensitivityPower;
	static InputSettings* GetInstance() {
		return *(InputSettings**)StaticOffsets::Get_OFFSET_INPUTSETTINGS();
	}
};