#include "pch.hpp"

std::vector<fvec3> points;
fvec3 ref_org;

long __stdcall Renderer::EndSceneRenderer(IDirect3DDevice9* device)
{
	static Renderer& renderer = Renderer::getInstance();
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_ENDSCENE);
	static Gui& gui = Gui::getInstance();

	if (GetAsyncKeyState(VK_NUMPAD0) & 1)
		IN_ActivateMouse(!cg::s_wmv->mouseActive);

	if (!ImGui::GetCurrentContext()) {
		renderer.initialize();
		std::cout << "create imgui\n";
		return detour_func.cast_call<long(__stdcall*)(IDirect3DDevice9*)>(device);

	}

	if (renderer.begin_frame()) {
		
	//	ImGui::GetBackgroundDrawList()->AddText(ivec2(100, 100), 0xFF0000FF, "yo fuckers xD");

		T_RenderAssistBall();

		gui.render();		

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

	sprintf_s(buffer, "%i", (int)fvec2(clients->cgameVelocity).mag());

	R_AddCmdDrawTextWithEffects(buffer, "fonts/objectivefont", cgs->refdef.width/2.f-15.f, cgs->refdef.height/2.f-15.f, 1.3f, 1.3f, 0.f, vec4_t{ 0,1,0,1 }, 3, glowCol, nullptr, nullptr, 0, 0, 0, 0);


	return detour_func.cast_call<void (__cdecl*)()>();
}