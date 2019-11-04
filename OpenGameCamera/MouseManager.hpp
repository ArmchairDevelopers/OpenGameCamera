#include <Windows.h>

// code to manage our mouse hook, and set the mouse state

class MouseManager {
public:
	// arg 2 of the hooked function
	class MouseStateObject {
	public:
		char pad[8];
		DWORD value;
	};
	// typdef it
	typedef void(*__fastcall tSetMouseState)(__int64, MouseStateObject*);
	static tSetMouseState oSetMouseState;

	// we need to hook it to steal arg1
	static void __fastcall hkSetMouseState(__int64 a1, MouseStateObject* a2)
	{
		// set our arg1, then return the original
		MouseManager::arg1 = a1;
		oSetMouseState(a1, a2);
	}
	// the argument wee need to steal
	static __int64 arg1;
public:
	// set the mouse state
	static bool SetMouseState(bool enableMouse) {
		// if arg 1 hasn't been stolen yet because the function hasn't been called, return false
		if (MouseManager::arg1 == NULL) return false;

		// setup our object
		MouseStateObject mso;

		// the two different mouse states, I pulled them from IDA
		const DWORD DISABLEMOUSE = 3652316602;
		const DWORD ENABLEMOUSE = 3325954792;

		// set the mouse state accordingly
		mso.value = enableMouse ? ENABLEMOUSE : DISABLEMOUSE;

		// call the original function for setting mouse state, then return true
		oSetMouseState(MouseManager::arg1, &mso);
		return true;
	}
};

// make our arg NULL by default
__int64 MouseManager::arg1 = NULL;
MouseManager::tSetMouseState MouseManager::oSetMouseState = nullptr;