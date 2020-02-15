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
	char pad[0x5c];
	float forceFov;
};

class GlobalPostProcessSettings {
public:
	char pad_0000[196]; //0x0000
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
	char pad_00FC[313]; //0x00FC
	bool spriteDofEnable; //0x0235
	char pad_0236[1]; //0x0236
	bool enableForeground; //0x0237
	char pad_0238[2]; //0x0238
	bool spriteDofHalfResolutionEnable; //0x023A
	bool spriteDofNearGatherEnable; // 0x23b
	bool spriteDofBestUpsamplingEnable; // 0x23c
	bool spriteDofMultilayerForegroundEnable; // 0x23d
	bool spriteDofPackedBokehEnable; // 0x23e
	bool spriteDofBicubicSampleEnable; // 0x23f
	bool spriteDofDebugEnable; // 0x240
	bool spriteDofUseAsyncCompute; // 0x241
	bool spriteDofOpticalVignettingEnable; // 0x242
	bool circularDofEnable; // 0x243
	bool circularDofEnableHighRes; // 0x244
	bool circularDofEnableFarBlurHighQuality; // 0x245
	bool circularDofEnableAntiBanding; // 0x246
	bool dynamicAOEnable; // 0x247
	bool ssaoBlurEnable; // 0x248
	bool screenSpaceRaytraceEnable; // 0x249
	bool screenSpaceRaytraceDeferredResolveEnable; // 0x24a
	bool screenSpaceRaytraceUseVelocityVectorsForTemporal; // 0x24b
	bool screenSpaceRaytraceSeparateCoverageEnable; // 0x24c
	bool screenSpaceRaytraceFullresEnable; // 0x24d
	bool screenSpaceRaytraceCameraCutEnable; // 0x24e
	bool screenSpaceRaytraceAsyncComputeEnable; // 0x24f
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