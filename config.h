#pragma once

class configuration {
public:
	bool triggerbotcfg = false;
	bool triggerbotFlashCheck = false;
	bool triggerbotScopedCheck = false;
	bool triggerbotAutoZoom = false;
	bool bunnyhopcfg = false;
	bool glowcfg = false;
	bool legitChams = false;
	bool boxesp = false;
	bool radarHack = false;
	bool flashHack = false;
	bool noArms = false;
	bool thirdperson = false;
	bool betterCrosshair = false;
};



float glowChams[3]; //RGB 

float chams[3];

float customTimeFlash;

float sniperColor[] = { 255, 255, 255, 255 };

float boxColor[] = { 255,255,255,255 };

inline D3DCOLOR FLOAT4TOD3DCOLOR(float Col[])
{
	ImU32 col32_no_alpha = ImGui::ColorConvertFloat4ToU32(ImVec4(Col[0], Col[1], Col[2], Col[3]));
	float a = (col32_no_alpha >> 24) & 255;
	float r = (col32_no_alpha >> 16) & 255;
	float g = (col32_no_alpha >> 8) & 255;
	float b = col32_no_alpha & 255;
	return D3DCOLOR_ARGB((int)a, (int)b, (int)g, (int)r);
}
