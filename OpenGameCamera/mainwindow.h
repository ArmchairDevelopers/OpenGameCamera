#pragma once

#include "window.h"

class MainWindow : public Window
{
public:
	MainWindow();
	~MainWindow();
	void Draw() override;
};

static MainWindow* gpMainWindow;
