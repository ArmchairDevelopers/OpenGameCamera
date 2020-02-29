#include "SigScan.h"

// macro for lazily adding signatures to this sig list
#define AddOffset(name, pattern, type) \
static DWORD64 Get_## name () {\
	static DWORD64 name = NULL;\
	if (name != NULL) return name; \
	return name=(DWORD64)PatternScanner::FindPattern({pattern, type});\
} 

// same but to resolve a call (call func)
#define AddFuncCall(name, pattern, type) \
static DWORD64 Get_## name () {\
	static DWORD64 name = NULL;\
	if (name != NULL) return name; \
	DWORD64 val = (DWORD64)PatternScanner::FindPattern({pattern, type}); \
	return name = val + 5 + (*(int*)(val + 1)); \
} 


class StaticOffsets {
public:
	AddOffset(OFFSET_GAMERENDERER, "48 8B 0D [?? ?? ?? ?? 48 85 C9 74 0E 48 8B 01 48 8D 15 ?? ?? ??", PatternType::RelativePointer)
	AddOffset(OFFSET_FIRSTTYPEINFO, "48 8B 05 [?? ?? ?? ?? 48 89 41 08 48 89 0D ?? ?? ?? ??", PatternType::RelativePointer)
	AddOffset(OFFSET_DXRENDERER, "48 8B 0D [?? ?? ?? ?? 44 89 74 24 50 44 89 74", PatternType::RelativePointer)
	AddOffset(OFFSET_UISETTINGS, "48 89 05 [?? ?? ?? ?? 48 8B 48 28 E8 ?? ?? ?? ?? 41 0F B6 D4", PatternType::RelativePointer)
	AddOffset(OFFSET_GAMETIMESETTINGS, "48 89 05 [?? ?? ?? ?? C7 40 ?? ?? ?? ?? ?? 8B 43 18", PatternType::RelativePointer)
	AddOffset(OFFSET_INPUTSETTINGS, "48 89 05 [?? ?? ?? ?? 80 B8 A5 00 00 00 00 F3 0F 10 35 ?? ?? ?? ?? 74 ??", PatternType::RelativePointer)
	AddOffset(OFFSET_KEYBOARDUPDATE, "[48 89 5C 24 08 57 48 83 EC 20 89 D7 48 89 CB 4D 85 C0 75 ?? E8 ?? ?? ?? ?? 48 89 C1 E8 ?? ?? ?? ?? 49 89 C0 83 BB B8 00 00 00 00 75 ?? 49 8B 00 4C 89 C1 40 0F B6 D7 FF 50 30 84 C0 74 ?? B8 01 00 00 00", PatternType::Address)
	AddOffset(OFFSET_SETMOUSESTATE, "[53 48 83 EC 20 48 89 CB 48 8B 0D 01 E5 29 FD 48 85 C9", PatternType::Address)
	AddOffset(OFFSET_POSTPROCESSSUB, "[41 8B 80 88 00 00 00 89 81 88 00 00 00 41 8B 80 8C 00 00 00 89 81 8C 00 00 00 41 0F B6 80 14 02 00 00", PatternType::Address)

	AddFuncCall(OFFSET_DEBUGRENDERER2, "[E8 ?? ?? ?? ?? 48 8B 97 30 79 00 00", PatternType::Address)
	AddFuncCall(OFFSET_DRAWLINE, "[E8 ?? ?? ?? ?? FF C3 8B 0D ?? ?? ?? ?? FF C9", PatternType::Address)
	AddFuncCall(OFFSET_DRAWRECT2D, "[E8 ?? ?? ?? ?? C7 87 ?? ?? 00 00 37", PatternType::Address)
	AddFuncCall(OFFSET_DRAWTEXT, "[E8 ?? ?? ?? ?? 83 C7 14 48 8D 76 04 4D 8D 76 08 49 83 ED 01", PatternType::Address)
	AddFuncCall(OFFSET_CAMERAHOOK2, "[E8 ?? ?? ?? ?? 4C 8B 4C 24 28 41 0F B6 41 07", PatternType::Address)
};

// to-do: generate signatures for these
//#define OFFSET_INPUTSETTINGS 0x14446D680
// 48 89 05 [?? ?? ?? ?? 80 B8 A5 00 00 00 00 F3 0F 10 35 ?? ?? ?? ?? 74 ??

//#define OFFSET_CAMERAHOOK2 0x140ACBA20
// E8 [?? ?? ?? ?? 48 03 DE 48 8B D3 48 8D 8D A0 2D 00 00

//#define OFFSET_KEYBOARDUPDATE 0x146937FA0
// [48 89 5C 24 08 57 48 83 EC 20 89 D7 48 89 CB 4D 85 C0 75 ?? E8 ?? ?? ?? ?? 48 89 C1 E8 ?? ?? ?? ?? 49 89 C0 83 BB B8 00 00 00 00 75 ?? 49 8B 00 4C 89 C1 40 0F B6 D7 FF 50 30 84 C0 74 ?? B0 01

//#define OFFSET_SETMOUSESTATE 0x14753ED00
// [53 48 83 EC 20 48 89 CB 48 8B 0D ?? ?? ?? ?? 48 85 C9 0F 84

//#define OFFSET_POSTPROCESSSUB 0x142E93F40
// [41 8B 80 88 00 00 00 89 81 88 00 00 00 41 8B 80 8C 00 00 00 89 81 8C 00 00 00 41 0F B6 80 14 02 00 00


#undef AddFuncCall
#undef AddOffset