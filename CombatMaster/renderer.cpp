#include "renderer.h"

void renderer::drawLine(const ImVec2& a, const ImVec2& b, ImU32 color, bool shadow)
{
	if (shadow)
		ImGui::GetForegroundDrawList()->AddLine(a, b, IM_COL32_BLACK, 3.f);

	ImGui::GetForegroundDrawList()->AddLine(a, b, color, 1.f);
}

void renderer::drawText(const ImVec2& a, ImU32 color, const std::string& text, bool shadow)
{
	if (shadow)
	{
		ImGui::GetForegroundDrawList()->AddText(a + ImVec2(0.f, 1.f), IM_COL32_BLACK, text.c_str());
		ImGui::GetForegroundDrawList()->AddText(a - ImVec2(0.f, 1.f), IM_COL32_BLACK, text.c_str());
		ImGui::GetForegroundDrawList()->AddText(a - ImVec2(1.f, 0.f), IM_COL32_BLACK, text.c_str());
		ImGui::GetForegroundDrawList()->AddText(a + ImVec2(1.f, 0.f), IM_COL32_BLACK, text.c_str());
	}

	ImGui::GetForegroundDrawList()->AddText(a, color, text.c_str());
}
void renderer::drawCircle(float x_center, float y_center, float radius)
{
	ImGui::GetWindowDrawList()->AddCircle(ImVec2(x_center, y_center), radius, IM_COL32(255, 255, 0, 255), 32, 2.0f);
}
