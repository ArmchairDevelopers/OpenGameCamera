#include "mainwindow.h"
#include <Windows.h>

MainWindow::MainWindow()
{
	if (gpMainWindow)
	{
		MessageBox(0, "MainWindow is already initialized.", "Error", MB_ICONERROR);
		ExitProcess(EXIT_FAILURE);
	}
	gpMainWindow = this;
}

void MainWindow::Draw()
{
	//ImGui::BeginMainMenuBar();
	ImGui::Begin("OpenGameCamera", &bShowWindow, ImGuiWindowFlags_AlwaysAutoResize);
	//if (ImGui::BeginMenu("Home"))
	//{
		ImGui::Checkbox(std::string("Show Menu [" + Keys::showMenuKey.name + "]").c_str(), &bShowWindow);
		ImGui::Checkbox(std::string("Enable FreeCam [" + Keys::enableFreeCam.name + "]").c_str(), &Settings::enableFreeCam);
		ImGui::Checkbox(std::string("Disable UI [" + Keys::disableUi.name + "]").c_str(), &Settings::disableUi);
		ImGui::Checkbox("Show Camera Settings", &Settings::cameraMenu);
		ImGui::Checkbox("Show DOF Menu", &Settings::dofMenu);
		ImGui::Checkbox("Show Effects Menu", &Settings::effectsMenu);
		ImGui::Button(std::string("Reset Menu Positions [" + Keys::resetSettingsMenus.name + "]").c_str());
	//	ImGui::EndMenu();
	//}
	ImGui::End();
}