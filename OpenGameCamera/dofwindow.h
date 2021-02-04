#pragma once

#include "window.h"

class DofWindow : public Window
{
public:
	DofWindow();
	~DofWindow();
	void Draw() override;
	bool IsEnabled() override;
	void SetEnabled(bool enabled) override;
};
