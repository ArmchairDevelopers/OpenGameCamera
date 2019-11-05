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
	AddOffset(OFFSET_CAMERAHOOK, "49 8B D6 48 C1 E3 08 48 03 9F ?? ?? ?? ?? 48 8B CB E8 [?? ?? ?? ?? 4C 8B 4C 24 28 41 0F B6 41 07", PatternType::RelativePointer)
	AddOffset(OFFSET_DXRENDERER, "48 8B 0D [?? ?? ?? ?? 48 8D 55 C0 4C 8B 75 C0", PatternType::RelativePointer)
	AddOffset(OFFSET_GAMETIMESETTINGS, "48 89 05 [?? ?? ?? ?? C7 40 ?? ?? ?? ?? ?? 8B 43 18", PatternType::RelativePointer)
	
	AddFuncCall(OFFSET_DEBUGRENDERER2, "[E8 ?? ?? ?? ?? 4C 8B F0 48 85 C0 0F ?? ?? ?? ?? ?? F3 0F 10 05 ?? ?? ?? ?? F3", PatternType::Address)
	AddFuncCall(OFFSET_DRAWLINE, "[E8 ?? ?? ?? ?? FF C3 8B 0D ?? ?? ?? ?? FF C9", PatternType::Address)
	AddFuncCall(OFFSET_DRAWRECT2D, "[E8 ?? ?? ?? ?? C7 87 ?? ?? 00 00 37", PatternType::Address)
	AddFuncCall(OFFSET_DRAWTEXT, "[E8 ?? ?? ?? ?? 83 C7 14 48 8D 76 04 4D 8D 76 08 49 83 ED 01", PatternType::Address)
};

// to-do: generate signatures for these
#define OFFSET_INPUTSETTINGS 0x1444613d0
#define OFFSET_CAMERAHOOK2 0x140AC9D40
#define OFFSET_KEYBOARDUPDATE 0x1467a04a0
#define OFFSET_SETMOUSESTATE 0x1474DA990
#define OFFSET_POSTPROCESSSUB 0x0142E8D0A0

#undef AddFuncCall
#undef AddOffset