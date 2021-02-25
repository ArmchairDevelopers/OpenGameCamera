#define NOMINMAX
#include "postwindow.h"
#include <Windows.h>
#include <algorithm>

PostWindow::PostWindow()
{
	printf("Initializing PostWindow\n");
}

bool PostWindow::IsEnabled()
{
	return Settings::homeMenu && Settings::postMenu;
}

void PostWindow::SetEnabled(bool enabled)
{
	Settings::cameraMenu = enabled;
}

void PostWindow::Draw()
{
	ImGui::Begin("Post Process", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	//ImGui::Checkbox("Force Post Process Settings", &Settings::enablePost);
	ImGui::Checkbox("Enable Bloom", &Settings::forceBloomEnable);
	ImGui::Checkbox("Enable Vignette", &Settings::VignetteEnable);
	ImGui::Checkbox("Enable Color Grading", &Settings::enableGrading);
	ImGui::Checkbox("Enable Film Grain", &Settings::FilmGrainEnable);
	ImGui::Checkbox("Enable Chromatic Aberration", &Settings::ChromaticAberrationAllowed);
	ImGui::Checkbox("Enable Lens Distortion", &Settings::LensDistortionAllowed);
	ImGui::End();
}