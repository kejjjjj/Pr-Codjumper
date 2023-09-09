#include "pch.hpp"

std::vector<fvec3> points;
fvec3 ref_org;

long __stdcall Renderer::EndSceneRenderer(IDirect3DDevice9* device)
{
	static Renderer& renderer = Renderer::getInstance();
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_ENDSCENE);
	static Gui& gui = Gui::getInstance();

	if (!ImGui::GetCurrentContext()) {
		renderer.initialize();
		std::cout << "create imgui\n";
		return detour_func.cast_call<long(__stdcall*)(IDirect3DDevice9*)>(device);

	}

	if (renderer.begin_frame()) {
		
		//ImGui::GetBackgroundDrawList()->AddText(ivec2(100, 100), 0xFF0000FF, std::to_string(ps_loc->delta_angles[YAW]).c_str());

		T::Visuals::T_RenderAssistBall();

		gui.render();		

		//if (GetAsyncKeyState(VK_NUMPAD5) & 1) {
		//	int idx = int(random(cm->numBrushes));
		//	while (!&cm->brushes[idx] /*||cm->brushes[idx].numsides != 0*/
		//		/* std::string(cm->materials[cm->brushes[idx].axialMaterialNum[0][0]].material).find("clip") == std::string::npos*/) {
		//		idx = int(random(cm->numBrushes));
		//		//srand(time(NULL));
		//		//if (&cm->brushes[idx]) {

		//		//	if (cm->brushes[idx].numsides == 0)
		//		//		continue;

		//		//}
		//	}
		//	
		//	brushWindings.clear();
		//	CM_GetPolys(&cm->brushes[idx]);
		//	Com_Printf(CON_CHANNEL_OBITUARY, "index: ^2%i\n", idx);
		//	VectorCopy(cm->brushes[idx].maxs, ps_loc->origin);
		//	
		//}

		//if (!brushWindings.empty()) {

		//	const auto r = [](winding_t* w) {
		//		int idx = 1;
		//		int triIdx = 2;
		//		float begin[3] = { w->p[0][0], w->p[0][1], w->p[0][2] };
		//		float* end;

		//		float* a, *b, *c;

		//		a = begin;
		//		b = w->p[1];
		//		c = w->p[2];

		//		//int total = std::clamp(w->numpoints,0, 4);
		//		int total = w->numpoints - 1;
		//		do{
		//			end = w->p[idx];
		//			R_DrawTriangle(a, b, c, vec4_t{0,255,0,55});
		//			R_DrawLine(begin, end, vec4_t{0,255,0,170});

		//			auto b1 = WorldToScreen(begin);
		//			auto e1 = WorldToScreen(end);

		//			if (b1 && e1) {
		//				ImGui::GetBackgroundDrawList()->AddText(b1.value(), IM_COL32(255, 0, 0, 255), (std::to_string(idx)).c_str());
		//				//ImGui::GetBackgroundDrawList()->AddText(e.value(), IM_COL32(255, 0, 0, 255), (std::to_string(idx)).c_str());


		//			}

		//			--total;
		//			++idx;
		//			++triIdx;
		//			a = c;
		//			b = w->p[triIdx];
		//			c = w->p[triIdx];
		//			

		//			VectorCopy(end, begin);

		//			//if(idx == 3)
		//			//	break;
		//		} while (total);

		//		auto ah = WorldToScreen(w->p[--idx]);
		//		if (ah) 
		//			ImGui::GetBackgroundDrawList()->AddText(ah.value(), IM_COL32(255, 0, 0, 255), "last");



		//		//R_DrawLine(w->p[0], end, vec4_t{ 0,255,0,170 });


		//	};


		//	int idx = 0;

		//
		//	static int winding = 0;

		//	if (GetAsyncKeyState(VK_UP) & 1) {
		//		winding++;
		//		if (brushWindings.size()-1 < winding)
		//			--winding;

		//		Com_Printf(CON_CHANNEL_OBITUARY, "winding[^2%i^7]\n", winding);
		//	}
		//	else if (GetAsyncKeyState(VK_DOWN) & 1) {
		//		if (winding > 0)
		//			winding--;
		//		Com_Printf(CON_CHANNEL_OBITUARY, "winding[^1%i^7]\n", winding);

		//	}

		//	auto it = brushWindings.begin();
		//	std::advance(it, winding);

		//	r(&*it);

		//	//for (auto it = brushWindings.begin(); it != brushWindings.end(); it++) {
		//	//	r(&*it);
		//	//}

		//	//auto b = WorldToScreen(brushWindings.begin()->p[1]);
		//	//auto c = WorldToScreen(brushWindings.begin()->p[0]);

		//	//if (b && c) {
		//	//	ImGui::GetBackgroundDrawList()->AddText(b.value(), IM_COL32(0, 255, 0, 255), (std::string("FRONT ") + std::to_string(idx)).c_str());
		//	//	ImGui::GetBackgroundDrawList()->AddText(c.value(), IM_COL32(0, 255, 0, 255), (std::string("END ") + std::to_string(idx)).c_str());

		//	//}
		//	//if (auto p = WorldToScreen(brushPoints_l.front())) {

		//	//	ImGui::GetBackgroundDrawList()->AddCircleFilled(p.value(), 3, IM_COL32(0, 255, 0, 255));

		//	//}

		//}
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
		int fps = T::Movement::T_GetIdealFPS(pm_glob, pml_glob);
		sprintf_s(buffer, "%i", fps);
		R_AddCmdDrawTextWithEffects(buffer, "fonts/objectivefont", cgs->refdef.width / 2.f - 15.f, cgs->refdef.height / 2.f + 30.f, 1.3f, 1.3f, 0.f, vec4_t{ 1,1,0,1 }, 3, glowCol, nullptr, nullptr, 0, 0, 0, 0);
	}

	return detour_func.cast_call<void (__cdecl*)()>();
}