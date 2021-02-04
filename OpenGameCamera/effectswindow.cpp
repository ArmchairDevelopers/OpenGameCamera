#include "effectswindow.h"
#include <Windows.h>

EffectsWindow::EffectsWindow()
{
	printf("Initializing EffectsWindow\n");
}

bool EffectsWindow::IsEnabled()
{
	return Settings::homeMenu && Settings::effectsMenu;
}

void EffectsWindow::SetEnabled(bool enabled)
{
	Settings::effectsMenu = enabled;
}

void EffectsWindow::Draw()
{
	ImGui::Begin("Effects Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox(std::string("Freeze Time [" + Keys::freezeTime.name + "]").c_str(), &Settings::freezeTime);
	ImGui::SliderFloat("Time scale", &Settings::timeScale, 0.1, 1.f, "%.3f", 1.0F);
	ImGui::Checkbox(std::string("Freeze Player [" + Keys::freezePlayer.name + "]").c_str(), &Settings::freezePlayer);
	ImGui::Checkbox("Bloom Enable", &Settings::forceBloomEnable);
	ImGui::Checkbox("SSR Enable", &Settings::ssrEnable);
	ImGui::Checkbox("SSR Full Res Enable", &Settings::ssrFullResEnable);
	ImGui::End();
}