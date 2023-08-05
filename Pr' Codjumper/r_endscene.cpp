#include "pch.hpp"


long __stdcall Renderer::EndSceneRenderer(IDirect3DDevice9* device)
{
	static Renderer& renderer = Renderer::getInstance();
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_ENDSCENE);
	
	if (renderer.begin_frame()) {

		ImGui::GetBackgroundDrawList()->AddText(ivec2(100, 100), 0xFF0000FF, "yo fuckers xD");

		renderer.end_frame();
	}

	return detour_func.cast_call<long(__stdcall*)(IDirect3DDevice9*)>(device);
}

void __cdecl Renderer::CG_DrawActive()
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_DRAWACTIVE);

	char buffer[128];

	sprintf_s(buffer, "x:     %.6f\ny:     %.6f\nz:     %.6f\nyaw: %.6f", clients->cgameOrigin[0], clients->cgameOrigin[1], clients->cgameOrigin[2], cgs->refdefViewAngles[YAW]);

	float col[4] = { 0,1,0,1 };
	float glowCol[4] = { 0,0,0,0 };


	R_AddCmdDrawTextWithEffects(buffer, "fonts/normalfont", 10.f, 480.f, 1.3f, 1.3f, 0.f, col, 3, glowCol, nullptr, nullptr, 0,0,0,0);

	return detour_func.cast_call<void (__cdecl*)()>();
}