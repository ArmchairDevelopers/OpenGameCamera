#pragma once

#include "window.h"

class CameraWindow : public Window
{
public:
	CameraWindow();
	~CameraWindow();
	void Draw() override;
	bool IsEnabled() override;
	void SetEnabled(bool enabled) override;
};
