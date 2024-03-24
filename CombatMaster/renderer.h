#pragma once

#include <string>

#include "MinHook.h"
#pragma comment(lib, "libMinHook.x64.lib")

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "kiero.h"

namespace renderer
{
	void drawLine(const ImVec2& a, const ImVec2& b, ImU32 color, bool shadow = true);
	void drawText(const ImVec2& a, ImU32 color, const std::string& text, bool shadow = true);
	void drawCircle(float x_center, float y_center, float radius);

}
