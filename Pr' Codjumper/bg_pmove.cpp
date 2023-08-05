
#include "pch.hpp"

void PM_AirMove(pmove_t* pm, pml_t* pml)
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_PM_AIRMOVE);

	memcpy(&pm_glob, pm, sizeof(pmove_t));
	memcpy(&pml_glob, pml, sizeof(pml_t));

	//if (const auto yaw = CG_GetOptYawDelta(pm, pml)) {
	//	opt_angle = yaw.value();
	//	//pm->cmd.angles[YAW] += ANGLE2SHORT(yaw.value());
	//	pm->ps->viewangles[YAW] -= yaw.value();

	//	auto fwd = fvec3(pm->ps->viewangles).toforward();
	//	auto rght = fvec3(pm->ps->viewangles).toright();
	//	auto up = fvec3(pm->ps->viewangles).toup();

	//	pml->forward[0] = fwd.x;
	//	pml->forward[1] = fwd.y;
	//	pml->forward[2] = fwd.z;

	//	pml->right[0] = rght.x;
	//	pml->right[1] = rght.y;
	//	pml->right[2] = rght.z;

	//	pml->up[0] = up.x;
	//	pml->up[1] = up.y;
	//	pml->up[2] = up.z;

	//	//AngleVectors(pm->ps->viewangles, pml->forward, pml->right, pml->up);
	//}

	return detour_func.cast_call<void(__cdecl*)(pmove_t* pm, pml_t* pml)>(pm, pml);
}
void PM_WalkMove(pmove_t* pm, pml_t* pml)
{
	memcpy(&pm_glob,pm, sizeof(pmove_t));
	memcpy(&pml_glob, pml, sizeof(pml_t));

	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_PM_WALKMOVE);



	//if (const auto yaw = CG_GetOptYawDelta(pm, pml)) {
	//	opt_angle = yaw.value();
	//	//pm->cmd.angles[YAW] += ANGLE2SHORT(yaw.value());
	//	pm->ps->viewangles[YAW] -= yaw.value();

	//	auto fwd = fvec3(pm->ps->viewangles).toforward();
	//	auto rght = fvec3(pm->ps->viewangles).toright();
	//	auto up = fvec3(pm->ps->viewangles).toup();

	//	pml->forward[0] = fwd.x;
	//	pml->forward[1] = fwd.y;
	//	pml->forward[2] = fwd.z;

	//	pml->right[0] = rght.x;
	//	pml->right[1] = rght.y;
	//	pml->right[2] = rght.z;

	//	pml->up[0] = up.x;
	//	pml->up[1] = up.y;
	//	pml->up[2] = up.z;

	//	//AngleVectors(pm->ps->viewangles, pml->forward, pml->right, pml->up);
	//}

	return detour_func.cast_call<void(__cdecl*)(pmove_t * pm, pml_t * pml)>(pm, pml);
}