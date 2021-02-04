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
	ImGui::Begin("DOF Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Enable DOF effects", &Settings::enableDof);
	ImGui::Checkbox("DOF Enable Foreground", &Settings::dofEnableForeground);
	ImGui::SliderFloat("DOF Max Blur", &Settings::dofBlurMax, 0, 50, "%.3f", 1.0F);
	ImGui::SliderFloat("DOF Focus Distance", &Settings::focusDistance, 0, 10000, "%.3f", 1.0F);
	ImGui::SliderFloat("DOF Far Start", &Settings::dofFarStart, 0, 10000, "%.3f", 1.0F);
	ImGui::SliderFloat("DOF Far End", &Settings::dofFarEnd, 0, 10000, "%.3f", 1.0F);
	ImGui::SliderFloat("DOF Near Start", &Settings::dofNearStart, 0, 10000, "%.3f", 1.0F);
	ImGui::SliderFloat("DOF Near End", &Settings::dofNearEnd, 0, 10000, "%.3f", 1.0F);
	ImGui::Checkbox("DOF Sprite Half-Resolution", &Settings::spriteHalfResolution);
	ImGui::End();
}