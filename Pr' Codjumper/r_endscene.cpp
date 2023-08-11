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
		
		//ImGui::GetBackgroundDrawList()->AddText(ivec2(100, 100), 0xFF0000FF, std::to_string(ps_loc->delta_angles[YAW]).c_str());

		T::Visuals::T_RenderAssistBall();

		gui.render();		

		//ImGui::Begin("##huuuh", 0, /*ImGuiWindowFlags_AlwaysAutoResize | */ImGuiWindowFlags_NoTitleBar);

		//static ScrollingBuffer sdata1, sdata2;
		//static RollingBuffer   rdata1, rdata2;
		//ImVec2 mouse = ImGui::GetMousePos();
		//static float t = 0;
		//t += ImGui::GetIO().DeltaTime;

		//static fvec2& delta = PlayerMouse::get().delta;

		//sdata1.AddPoint(t, delta.x * 0.1f);
		//rdata1.AddPoint(t, delta.x * 0.1f);
		//sdata2.AddPoint(t, delta.y * 0.1f);
		//rdata2.AddPoint(t, delta.y * 0.1f);

		//static float history = 8.0f;
		//rdata1.Span = history;
		//rdata2.Span = history;

		//static ImPlotAxisFlags x_flags = ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels;
		//static ImPlotAxisFlags y_flags = ImPlotAxisFlags_AutoFit;

		//if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, 150), ImPlotFlags_NoMouseText | ImPlotFlags_Crosshairs)) {
		//	ImPlot::SetupAxes(nullptr, nullptr, x_flags, y_flags);
		//	ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
		//	ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);

		//	//ImPlot::SetupAxisTicks(ImAxis_Y1, )
		//	ImPlot::SetNextFillStyle(ImVec4(0,255,0,255), 1.f);
		//	ImPlot::PlotLine("##Mouse X", &sdata1.Data[0].x, &sdata1.Data[0].y, sdata1.Data.size(), 0, sdata1.Offset, 2 * sizeof(float));
		//	ImPlot::EndPlot();
		//}

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