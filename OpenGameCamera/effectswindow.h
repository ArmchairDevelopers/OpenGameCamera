#pragma once

#include "window.h"

class EffectsWindow : public Window
{
public:
	EffectsWindow();
	~EffectsWindow();
	void Draw() override;
	bool IsEnabled() override;
	void SetEnabled(bool enabled) override;
};
