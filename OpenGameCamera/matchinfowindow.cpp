#include "matchinfowindow.h"
#include <Windows.h>
#include "sdk.hpp"

MatchInfoWindow::MatchInfoWindow()
{
	printf("Initializing MatchInfoWindow\n");
}

bool MatchInfoWindow::IsEnabled()
{
	return Settings::homeMenu && Settings::matchInfoMenu;
}

void MatchInfoWindow::SetEnabled(bool enabled)
{
	Settings::matchInfoMenu = enabled;
}

void MatchInfoWindow::Draw()
{
	ImGui::Begin("Match Info", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	GameContext* context = GameContext::GetInstance();
	bool inLevel = context->GetPlayerManager()->GetLocalPlayer()->controlledControllable != nullptr;
	ImGui::Text("Level: %s", context->GetClientLevel()->GetLevelName());
	if (inLevel) {
		ImGui::Text("Game Mode: %s", context->GetClientLevel()->GetMatchInfo()->GetMatchMode());
	}
	ImGui::Text("");
	ImGui::Text("Local Player Info:");
	ImGui::Text("Name: %s", context->GetPlayerManager()->GetLocalPlayer()->GetName());
	if (inLevel) {
		ImGui::Text("Class: %s", context->GetPlayerManager()->GetLocalPlayer()->GetPlayerClassName());
		ImGui::Text("");
		for (int i = 0; i < 64; i++) {
			ClientPlayer* player = context->GetPlayerManager()->GetmPPlayer()->PlayerArray[i];
			if (player == nullptr || player->controlledControllable == nullptr) continue;

			Vec3 location = player->controlledControllable->clientSoldierPrediction->Location;

			ImGui::Text("%s (%.0f / %.0f / %.0f)", player->GetName(), location.x, location.y, location.z);
			const float ItemSpacing = ImGui::GetStyle().ItemSpacing.x;

			static float TeleportButtonWidth = 100.0f;
			float pos = TeleportButtonWidth + ItemSpacing;
			ImGui::SameLine(ImGui::GetWindowWidth() - pos);
			if (ImGui::SmallButton(std::string("Teleport##" + std::string(player->GetName())).c_str()))
			{
				Vec4 origin = GameRenderer::GetInstance()->renderView->transform.o;
				origin.x = location.x;
				origin.y = location.y;
				origin.z = location.z;
				origin.w = -1; // Hacky fix for resetting the camera position on drawLoop
				Globals::g_CameraPosition = origin;
				printf("Traveling to %s\n", player->GetName());
			}
			TeleportButtonWidth = ImGui::GetItemRectSize().x;
		}
	}
	ImGui::End();
}