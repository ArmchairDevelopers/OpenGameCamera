#pragma once

#include "window.h"

class MainWindow : public Window
{
public:
	MainWindow();
	~MainWindow();
	void Draw() override;
	bool IsEnabled() override;
	void SetEnabled(bool enabled) override;
};

static MainWindow* gpMainWindow;
