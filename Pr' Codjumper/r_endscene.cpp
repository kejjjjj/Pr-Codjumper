#include "pch.hpp"

std::vector<fvec3> points;
fvec3 ref_org;


long __stdcall Renderer::EndSceneRenderer(IDirect3DDevice9* device)
{
	static Renderer& renderer = Renderer::getInstance();
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_ENDSCENE);
	static Gui& gui = Gui::getInstance();
	static Prediction_Init& prediction = prediction.getInstance();

	const auto choose_render_function = [&](){

		if (prediction.ProjectExists()) {
			prediction.Hud_Render();
		}

		if (prediction.UI_ShouldRender())
			return prediction.UI_Render();

		return gui.render();
	};

	if (!ImGui::GetCurrentContext()) {
		renderer.initialize();
		std::cout << "create imgui\n";
		return detour_func.cast_call<long(__stdcall*)(IDirect3DDevice9*)>(device);

	}

	if (renderer.begin_frame()) {
		
		T::Visuals::T_RenderAssistBall();

		choose_render_function();

		renderer.end_frame();
	}

	return detour_func.cast_call<long(__stdcall*)(IDirect3DDevice9*)>(device);
}


void __cdecl Renderer::CG_DrawActive()
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_DRAWACTIVE);
	static Playback& playback = Playback::getInstance();
	static Prediction_Init& prediction = prediction.getInstance();

	if (!T::Movement::T_Ready())
		return detour_func.cast_call<void(__cdecl*)()>();

	char buffer[128];

	sprintf_s(buffer, "x:     %.6f\ny:     %.6f\nz:     %.6f\nyaw: %.6f", clients->cgameOrigin[0], clients->cgameOrigin[1], clients->cgameOrigin[2], AngleNormalize90(clients->cgameViewangles[YAW]));

	float col[4] = { 0,1,0,1 };
	float glowCol[4] = { 1,0,0,1 };


	R_AddCmdDrawTextWithEffects(buffer, "fonts/normalfont", 10.f, 480.f, 1.3f, 1.3f, 0.f, vec4_t{ 0,1,0,1 }, 3, vec4_t{0,0,0,0}, nullptr, nullptr, 0, 0, 0, 0);

	sprintf_s(buffer, "%i", (int)fvec2(clients->cgameVelocity).mag());

	R_AddCmdDrawTextWithEffects(buffer, "fonts/objectivefont", cgs->refdef.width/2.f-15.f, cgs->refdef.height/2.f-15.f, 1.3f, 1.3f, 0.f, col, 3, glowCol, nullptr, nullptr, 0, 0, 0, 0);


	if (find_evar<bool>("AutoFPS")->get()) {
		int fps = T::Movement::T_GetIdealFPS(pm_glob);
		sprintf_s(buffer, "%i", fps);
		R_AddCmdDrawTextWithEffects(buffer, "fonts/objectivefont", cgs->refdef.width / 2.f - 15.f, cgs->refdef.height / 2.f + 30.f, 1.3f, 1.3f, 0.f, vec4_t{ 1,1,0,1 }, 3, glowCol, nullptr, nullptr, 0, 0, 0, 0);
	}

	auto results = T::Movement::GetDistanceToFPSZone(pm_glob, 333);
	sprintf_s(buffer, "%.2f, %.2f, (%.2f)", results.begin, results.end, results.length);
	R_AddCmdDrawTextWithEffects(buffer, "fonts/smallDevFont", cgs->refdef.width / 2.f - 15.f, cgs->refdef.height / 2.f + 100.f, 2.3f, 2.3f, 0.f, vec4_t{ 1,1,0,1 }, 3, glowCol, nullptr, nullptr, 0, 0, 0, 0);


	prediction.Hud_EngineRender();
	playback.DrawPlayback();

	return detour_func.cast_call<void (__cdecl*)()>();
}