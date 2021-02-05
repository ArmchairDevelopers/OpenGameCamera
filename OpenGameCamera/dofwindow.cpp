#include "dofwindow.h"
#include <Windows.h>

DofWindow::DofWindow()
{
	printf("Initializing DofWindow\n");
}

bool DofWindow::IsEnabled()
{
	return Settings::homeMenu && Settings::dofMenu;
}

void DofWindow::SetEnabled(bool enabled)
{
	Settings::dofMenu = enabled;
}

void DofWindow::Draw()
{
	ImGui::Begin("Depth of Field", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox(std::string("Enable Depth of Field [" + Keys::enableDof.name + "]").c_str(), &Settings::enableDof);
	ImGui::Checkbox("Enable Foreground Blur", &Settings::dofEnableForeground);
	ImGui::Checkbox("Blur Half Resolution", &Settings::spriteHalfResolution);
	ImGui::SliderFloat("Max Blur", &Settings::dofBlurMax, 0, 6, "%.3f", 1.0F);
	ImGui::SliderFloat("Near Start", &Settings::dofNearStart, 0, 32, "%.3f", 1.0F);
	ImGui::SliderFloat("Near End", &Settings::dofNearEnd, 0, 32, "%.3f", 1.0F);
	ImGui::SliderFloat("Focus Distance", &Settings::focusDistance, 0, 128, "%.3f", 1.0F);
	ImGui::SliderFloat("Far Start", &Settings::dofFarStart, 0, 32, "%.3f", 1.0F);
	ImGui::SliderFloat("Far End", &Settings::dofFarEnd, 0, 32, "%.3f", 1.0F);
	ImGui::End();
}