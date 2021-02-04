#pragma once

#include "window.h"
#include "renderer.h"

class CustomDraw : public Window
{
public:
	CustomDraw();
	~CustomDraw();
	void Setup(Renderer::tCustomDrawCallback callback);
	void Draw() override;
	bool IsEnabled() override;
};
