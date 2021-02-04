#include "camerawindow.h"
#include <Windows.h>

CameraWindow::CameraWindow()
{
	printf("Initializing CameraWindow\n");
}

bool CameraWindow::IsEnabled()
{
	return Settings::homeMenu && Settings::cameraMenu;
}

void CameraWindow::SetEnabled(bool enabled)
{
	Settings::cameraMenu = enabled;
}

void CameraWindow::Draw()
{
	ImGui::Begin("Camera Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::SliderFloat("Exposure Value", &Settings::evControl, 0, 5, "%.3f", 1.0F);
	ImGui::SliderFloat(std::string("FOV [" + Keys::fovDecrease.name + "] [" + Keys::fovIncrease.name + "]").c_str(), &Settings::fov, 0, 265, "%.3f", 1.0F); // Real max is 180, but forced it can go up to 265 before everything breaks and goes upside-down
	ImGui::SliderFloat(std::string("FreeCam Sensitivity").c_str(), &Settings::camSens, 0, 5, "%.3f", 1.0F);
	ImGui::Checkbox(std::string("Enable Resolution Scale (LAGGY) [" + Keys::enableResScale.name + "]").c_str(), &Settings::enableResScale);
	ImGui::SliderFloat("Resolution Scale", &Settings::resScale, 0, 5, "%.3f", 1.0F);
	ImGui::End();
}