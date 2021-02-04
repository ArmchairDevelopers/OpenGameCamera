#pragma once

#include "window.h"

class MatchInfoWindow : public Window
{
public:
	MatchInfoWindow();
	~MatchInfoWindow();
	void Draw() override;
	bool IsEnabled() override;
	void SetEnabled(bool enabled) override;
};
