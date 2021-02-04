#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include "BasicTypes.hpp"
#include "sdk.hpp"
#include <string>
#include "window.h"

#include <list>
#include "mainwindow.h"

class Renderer
{
public:
	typedef void(__fastcall* tCustomDrawCallback)();

	static std::list<Window*> pUiInstances;

	MainWindow* pMainWindow = nullptr;
	ID3D11Device* pCurrentDevice = nullptr;
	ID3D11DeviceContext* pCurrentContext = nullptr;
	ID3D11RenderTargetView* pCurrentView = nullptr;

	Renderer();
	~Renderer();

	static void RegisterEventHandler(Window* ui)
	{
		pUiInstances.push_back(ui);
	}
};

static Renderer* gpRenderer;