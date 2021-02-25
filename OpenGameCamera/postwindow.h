#pragma once

#include "window.h"

class PostWindow : public Window
{
public:
	PostWindow();
	~PostWindow();
	void Draw() override;
	bool IsEnabled() override;
	void SetEnabled(bool enabled) override;
};
