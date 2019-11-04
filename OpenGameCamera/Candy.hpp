#pragma once
#include <Windows.h>
#include <cstdint>
#include "Typedefs.hpp"
#include "minhook/MinHook.h"

// candy is our hooking library, basically just a wrapper for MinHook
class Candy {
public:
	// templated function for VTAble Hooking, creds to Speedi
	template <typename T>
	static void CreateVTableHook(void* pVTable, void* pHook, T& t, uint32_t iIndex) {
		DWORD64** ppVTable = (DWORD64**)pVTable;
		// get the VTable address, call VirtualProtect so we can write to it
		DWORD dwOld = 0;
		VirtualProtect((void*)((*ppVTable) + iIndex), sizeof(PDWORD64), PAGE_EXECUTE_READWRITE, &dwOld);
		// read the original pointer at our supplied index
		PBYTE pOrig = ((PBYTE)(*ppVTable)[iIndex]);
		// change the pointer to whatever we wanteded it to be
		(*ppVTable)[iIndex] = (DWORD64)pHook;
		// set the protection status back to how it was before
		VirtualProtect((void*)((*ppVTable) + iIndex), sizeof(PDWORD64), dwOld, &dwOld);
		// cast the original function pointer to our templated typedef
		t = (T)pOrig;
	}

	// create a standard hook
	static void CreateHook(DWORD64 address, void* pHook, void* original) {
		// if this is the first hook, initialize minhook
		static bool firstTime = true;
		if (firstTime) {
			MH_Initialize();
		}
		// then create the hook, and enable it
		MH_CreateHook((LPVOID)address, pHook, (LPVOID*)original);
		MH_EnableHook((LPVOID)address);
	}
	static void DestroyHook(DWORD64 address) {
		MH_DisableHook((LPVOID)address);
		MH_RemoveHook((LPVOID)address);
	}
};



//template void Candy::CreateVTableHook<tEndFrame>(void*, void*, tEndFrame&, uint32_t);
