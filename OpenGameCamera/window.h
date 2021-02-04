#pragma once

#include <string>
#include "Globals.hpp"
#include "imgui/imgui.h"

class Window
{
public:
	Window();
	virtual void Draw();
	virtual void Resize();
	virtual bool IsEnabled();
	virtual void SetEnabled(bool enabled);
};
