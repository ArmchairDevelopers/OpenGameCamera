#include "mainwindow.h"
#include <Windows.h>
#include "camerawindow.h"
#include "dofwindow.h"
#include "effectswindow.h"

MainWindow::MainWindow()
{
	if (gpMainWindow)
	{
		MessageBox(0, "MainWindow is already initialized.", "Error", MB_ICONERROR);
		ExitProcess(EXIT_FAILURE);
	}
	gpMainWindow = this;
	new CameraWindow();
	new DofWindow();
	new EffectsWindow();
}

bool MainWindow::IsEnabled()
{
	return Settings::homeMenu;
}

void MainWindow::SetEnabled(bool enabled)
{
	Settings::homeMenu = enabled;
}

void MainWindow::Draw()
{
	//ImGui::BeginMainMenuBar();
	ImGui::Begin("OpenGameCamera", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::Checkbox(std::string("Show Menu [" + Keys::showMenuKey.name + "]").c_str(), &Settings::homeMenu)) {
		Settings::updateMouseState = true;
	}
	ImGui::Checkbox(std::string("Enable FreeCam [" + Keys::enableFreeCam.name + "]").c_str(), &Settings::enableFreeCam);
	ImGui::Checkbox(std::string("Disable UI [" + Keys::disableUi.name + "]").c_str(), &Settings::disableUi);
	ImGui::Checkbox("Show Camera Settings", &Settings::cameraMenu);
	ImGui::Checkbox("Show DOF Menu", &Settings::dofMenu);
	ImGui::Checkbox("Show Effects Menu", &Settings::effectsMenu);
	ImGui::Checkbox("Show Information Menu", &Settings::informationMenu);
	ImGui::Button(std::string("Reset Menu Positions [" + Keys::resetSettingsMenus.name + "]").c_str());
	ImGui::End();
}