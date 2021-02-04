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

class VehicleEntityData
{
public:
	char pad_0000[568]; //0x0000
	char* VehicleName; //0x0238
	char pad_0240[520]; //0x0240

	char* GetName() {
		if (this != nullptr && this->VehicleName != nullptr) {
			return this->VehicleName;
		}
		return (char*)"\0";
	}
}; //Size: 0x0448

class AttachedControllable
{
public:
	char pad_0000[48]; //0x0000
	class VehicleEntityData* vehicleEntity; //0x0030
	char pad_0038[560]; //0x0038

	VehicleEntityData* GetVehicleEntityData() {
		if (this != nullptr && this->vehicleEntity != nullptr) {
			return this->vehicleEntity;
		}
	}

}; //Size: 0x1058


class SoldierBlueprint
{
public:
	char pad_0000[24]; //0x0000
	char* Name; //0x0018
	char pad_0020[40]; //0x0020
	char* GetName() {
		if (this != nullptr && this->Name != nullptr) {
			return this->Name;
		}
		return (char*)"\0";
	}
}; //Size: 0x0048

class ClientSoldierPrediction
{
public:
	char pad_0000[32]; //0x0000
	Vec3 Location; //0x0020
	char pad_002C[104]; //0x002C
}; //Size: 0x0094

class ClientSoldierEntity
{
public:
	char pad_0000[712]; //0x0000
	class ClientSoldierHealthComponent* ClientSoldierHealthComponent; //0x02C8
	char pad_02D0[104]; //0x02D0
	class SoldierBlueprint* soldierBlueprint; //0x0338
	char pad_0340[404]; //0x0340
	float HeightOffset; //0x04D4
	char pad_04D8[156]; //0x04D8
	float Yaw; //0x0574
	char pad_0578[4]; //0x0578
	float Pitch; //0x057C
	char pad_0580[472]; //0x0580
	ClientSoldierPrediction* clientSoldierPrediction; //0x0758
	char pad_0760[2488]; //0x0760

	SoldierBlueprint* GetSoldierBlueprint() {
		if (this != nullptr && this->soldierBlueprint != nullptr) {
			return this->soldierBlueprint;
		}
	}
}; //Size: 0x0840

class MatchInfo
{
public:
	char pad_0000[8]; //0x0000
	char* GameMode; //0x0008
	char pad_0010[1080]; //0x0010
	char* GetMatchMode() {
		if (this != nullptr && this->GameMode != nullptr) {
			return this->GameMode;
		}
		return (char*)"Menu";
	}
}; //Size: 0x0448

class ClientLevel
{
public:
	char pad_0000[48]; //0x0000
	class MatchInfo* matchInfo; //0x0030
	char* LevelName; //0x0038
	char pad_0040[1032]; //0x0040
	char* GetLevelName() {
		if (this != nullptr && this->LevelName != nullptr) {
			return this->LevelName;
		}
		return (char*)"\0";
	}
	MatchInfo* GetMatchInfo()
	{
		if (this != nullptr && this->matchInfo != nullptr)
		{
			return this->matchInfo;
		}
	}
}; //Size: 0x0148


class ClientPlayer
{
public:
	char pad_0000[24]; //0x0000
	char* Name; //0x0018
	char pad_0020[56]; //0x0020
	uint32_t Team; //0x0058
	char pad_005C[420]; //0x005C
	class AttachedControllable* attachedControllable; //0x0200
	char pad_0208[8]; //0x0208
	class ClientSoldierEntity* controlledControllable; //0x0210
	char pad_0218[4676]; //0x0218
	int32_t Score; //0x145C
	char pad_1460[4]; //0x1460
	int32_t Kills; //0x1464
	int32_t Assists; //0x1468
	int32_t Deaths; //0x146C
	char pad_1470[3028]; //0x1470

	ClientSoldierEntity* GetClientSoldier() {
		if (this != nullptr && this->attachedControllable == nullptr && this->controlledControllable != nullptr) {
			return this->controlledControllable;
		}
	}

	AttachedControllable* GetAttachedControllable() {
		if (this != nullptr && this->attachedControllable != nullptr) {
			return this->attachedControllable;
		}
	}

	uint32_t GetTeam() {
		if (this != nullptr && &this->Team != nullptr) {
			return this->Team;
		}
	}

	char* GetName() {
		if (this != nullptr && this->Name != nullptr) {
			return this->Name;
		}
		return (char*)"\0";
	}

	bool IsValid() {
		if (this != nullptr &&
			this->attachedControllable == nullptr &&
			this->controlledControllable != nullptr)
			return true;
		return false;
	}

	char* GetPlayerClassName() {
		if (IsValid()) {
			return this->GetClientSoldier()->GetSoldierBlueprint()->GetName();
		}
		//if (strcmp(outchar, "\0")) return outchar;
		return this->GetAttachedControllable()->GetVehicleEntityData()->GetName();

	}

}; //Size: 0x1040

class MppPlayer
{
public:
	class ClientPlayer* PlayerArray[64]; //0x0000
	char pad_01E0[64]; //0x01E0

	ClientPlayer* GetPlayerByID(int ID) {
		if (this != nullptr && this->PlayerArray[ID] != nullptr && this->PlayerArray[ID]->IsValid()) {
			return this->PlayerArray[ID];
		}
		return nullptr;
	}
}; //Size: 0x0220

class PlayerManager
{
public:
	char pad_0000[1384]; //0x0000
	class ClientPlayer* LocalPlayer; //0x0568
	char pad_0570[504]; //0x0570
	class MppPlayer* mppPlayer; //0x0768
	char pad_0770[6888]; //0x0770

	ClientPlayer* GetLocalPlayer() {
		if (this != nullptr && this->LocalPlayer != nullptr) {
			return this->LocalPlayer;
		}
	}
	MppPlayer* GetmPPlayer() {
		if (this != nullptr && this->mppPlayer != nullptr) {
			return this->mppPlayer;
		}
	}

}; //Size: 0x2258

class GameContext
{
public:
	char pad_0000[56]; //0x0000
	class ClientLevel* clientLevel; //0x0038
	char pad_0040[24]; //0x0040
	class PlayerManager* playerManager; //0x0058
	char pad_0060[232]; //0x0060

	ClientLevel* GetClientLevel() {
		if (this->clientLevel != nullptr) {
			return this->clientLevel;
		}
	}

	PlayerManager* GetPlayerManager() {
		if (this != nullptr && this->playerManager != nullptr) {
			return this->playerManager;
		}
	}

	static GameContext* GetInstance(void) {
		return *(GameContext**)(StaticOffsets::Get_OFFSET_CLIENTGAMECONTEXT());
	}
}; //Size: 0x0148