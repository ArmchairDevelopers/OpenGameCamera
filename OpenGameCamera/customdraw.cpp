#include "customdraw.h"

const ImGuiWindowFlags WINDOW_FLAGS = ImGuiWindowFlags_NoFocusOnAppearing |
									  ImGuiWindowFlags_AlwaysAutoResize   |
									  ImGuiWindowFlags_NoSavedSettings    |
								   	  ImGuiWindowFlags_NoDecoration       |
									  ImGuiWindowFlags_NoMove             |
									  ImGuiWindowFlags_NoNav;

const int CORNER = 0; /*
						0 = Top left
						1 = Top right
						2 = Bottom left
						3 = Bottom right
					  */

const float DISTANCE_TO_CORNER = 15.0f;

CustomDraw::CustomDraw()
{
	printf("Initializing CustomDraw\n");
}

Renderer::tCustomDrawCallback hkCallback = NULL;

void CustomDraw::Setup(Renderer::tCustomDrawCallback callback)
{
	hkCallback = callback;
}

bool CustomDraw::IsEnabled()
{
	return true;
}

void CustomDraw::Draw()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::SetNextWindowBgAlpha(0.45); // Make window background invisible.

	// Position the window.
	ImVec2 window_pos = ImVec2((CORNER & 1) ? io.DisplaySize.x - DISTANCE_TO_CORNER : DISTANCE_TO_CORNER, (CORNER & 2) ? io.DisplaySize.y - DISTANCE_TO_CORNER : DISTANCE_TO_CORNER);
	ImVec2 window_pos_pivot = ImVec2((CORNER & 1) ? 1.0f : 0.0f, (CORNER & 2) ? 1.0f : 0.0f);
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

	if (Settings::informationMenu) {
		ImGui::Begin("Information", reinterpret_cast<bool*>(true), WINDOW_FLAGS);
		{
			hkCallback();
			ImGui::End();
		}
	}
	else {
		hkCallback();
	}
}