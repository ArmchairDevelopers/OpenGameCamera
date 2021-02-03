#pragma once

#include <string>
#include "KeyMan.hpp"
#include "imgui/imgui.h"
#include "Settings.h"

class Window
{
public:
	bool bShowWindow = false;

	Window();
	virtual void Draw();
	virtual void Resize();
};
