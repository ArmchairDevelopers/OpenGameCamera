#define NOMINMAX
#include "camerawindow.h"
#include <Windows.h>
#include <algorithm>

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
	ImGui::Begin("Camera", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox(std::string("Freeze Time [" + Keys::freezeTime.name + "]").c_str(), &Settings::freezeTime);
	ImGui::Checkbox(std::string("Freeze Player [" + Keys::freezePlayer.name + "]").c_str(), &Settings::freezePlayer);
	ImGui::Checkbox("Lock FreeCam Location", &Settings::lockFreeCam);
	ImGui::SliderFloat(std::string("FOV [" + Keys::fovDecrease.name + "] [" + Keys::fovIncrease.name + "]").c_str(), &Settings::fov, 0, 265, "%.3f", 1.0F); // Real max is 180, but forced it can go up to 265 before everything breaks and goes upside-down
	ImGui::SliderFloat(std::string("FreeCam Sensitivity").c_str(), &Settings::camSens, 0, 5, "%.3f", 1.0F);
	ImGui::SliderFloat("Time scale", &Settings::timeScale, 0.1, 1.f, "%.3f", 1.0F);
	ImGui::Checkbox("Force Exposure", &Settings::forceEv);
	ImGui::SliderFloat("Exposure Value", &Settings::evControl, 0, 18, "%.3f", 1.0F);
	ImGui::Checkbox("Bloom Enable", &Settings::forceBloomEnable);
	ImGui::Checkbox("SSR Enable", &Settings::ssrEnable);
	ImGui::Checkbox("SSR Full Res Enable", &Settings::ssrFullResEnable);
	ImGui::Checkbox(std::string("Enable Resolution Scale [" + Keys::enableResScale.name + "]").c_str(), &Settings::enableResScale);
	ImGui::InputFloat("Resolution Scale", &Settings::resScale, 0.25f, 1.f, "%.3f", 1.0F);
	Settings::resScale = std::min(5.f, std::max(0.25f, Settings::resScale));
	ImGui::End();
}