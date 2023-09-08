#include "pch.hpp"


void T::Visuals::T_RenderAssistBall()
{

	if (NOT_SERVER)
		return;

	static auto evar = find_evar<bool>("Aim trainer");

	if (!evar)
		return;

	if (evar->get() == false)
		return;

	//if ((GetAsyncKeyState(VK_NUMPAD2) < 0) == false)
	//	return;

	fvec3 ball_origin;
	//trace_t trace;
	fvec3 fwd(rg->viewOrg);
	std::optional<float> op;

	fvec3 angles(clients->cgameViewangles);

	if (op = CG_GetOptYawDelta(pm_glob, pml_glob), !op)
		return;

	angles.y = pm_glob->ps->viewangles[YAW] - op.value();

	fwd = fwd + fvec3(angles).toforward() * 500;

	ball_origin = fwd;

	//CG_TracePoint(vec3_t{ 1,1,1 }, &trace, rg->viewOrg, vec3_t{ -1,-1,-1 }, vec3_t{ fwd.x, fwd.y, fwd.z }, cgs->clientNum, MASK_ALL, 1, 1);
	//ball_origin.x = rg->viewOrg[0] + trace.fraction * (fwd.x - rg->viewOrg[0]);
	//ball_origin.y = rg->viewOrg[1] + trace.fraction * (fwd.y - rg->viewOrg[1]);
	//ball_origin.z = rg->viewOrg[2] + trace.fraction * (fwd.z - rg->viewOrg[2]);
	
	std::list<fvec3> ball_points;
	Geom_CreateBall(ball_origin, 20, 30, 30, ball_points);

	if (ball_points.empty()) 
		return;

	if (ball_points.size() % 3 != 0)
		return;

	for (auto it = ball_points.begin(); it != ball_points.end();) {

		R_DrawTriangle(*it++, *it++, *it++, vec4_t{ 0,255,0,55 });

		//if (auto po = WorldToScreen(i)) {
		//	fvec2 p = po.value();
		//	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + 1, p.y + 1), ImColor{ 0.f,1.f,0.f,0.7f });
		//}
	}
	

}