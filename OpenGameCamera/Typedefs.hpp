#ifndef TYPEDEFS_DOT_HPP
#define TYPEDEFS_DOT_HPP
#include <Windows.h>

typedef DWORD64(__fastcall* tEndFrame)(void*, DWORD64 a2, bool presentEnable);
tEndFrame oEndFrame = nullptr;

typedef __int64(*__fastcall tupdateCamera2)(class CameraObject*, class CameraObject*);
tupdateCamera2 oupdateCamera2 = nullptr;

typedef bool(*__fastcall tkeyboardUpdate)(__int64, unsigned __int8, __int64);
tkeyboardUpdate okeyboardUpdate = nullptr;

#endif