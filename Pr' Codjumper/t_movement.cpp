#include "pch.hpp"

void T::Movement::T_Strafebot(pmove_t* pm, pml_t* pml)
{
	static decltype(auto) strafebot = find_evar<bool>("Strafebot");

	if (strafebot->get() == false)
		return;

	std::optional<float> yaw;


	if ((yaw = CG_GetOptYawDelta(pm, pml)) == std::nullopt)
		return;

	//pm->cmd.angles[YAW] += ANGLE2SHORT(yaw.value());
	pm->ps->viewangles[YAW] += yaw.value();

	fvec3(pm->ps->viewangles).toforward().assign_to(pml->forward);
	fvec3(pm->ps->viewangles).toright().assign_to(pml->right);
	fvec3(pm->ps->viewangles).toup().assign_to(pml->up);

	
}
void T::Movement::T_CorrectOverstrafe(pmove_t* pm, pml_t* pml)
{
	static decltype(auto) overstrafe_correction = find_evar<bool>("Overstrafe Correction");
	static float old_yaw = pm->ps->viewangles[YAW];

	if (overstrafe_correction->get() == false)
		return;

	std::optional<float> opt_yaw;

	if ((opt_yaw = CG_GetOptYawDelta(pm, pml)) == std::nullopt)
		return;

	const float delta = opt_yaw.value();
	const float user_delta = AngularDistance(pm->ps->viewangles[YAW], old_yaw);
	
	const float opt = pm->ps->viewangles[YAW] + delta;
	

	//don't correct if counter-strafing
	if (delta < 0 && user_delta >= 0 || user_delta < 0 && delta >= 0) {
		old_yaw = pm->ps->viewangles[YAW];
		return;
	}
		
	
	if(abs(user_delta) > 30)
		Com_Printf(CON_CHANNEL_OBITUARY, "^2 (%.2f, %.2f)\n", pm->ps->viewangles[YAW], old_yaw);



	if (pm->ps->groundEntityNum == 1023 && (user_delta > 0 && user_delta > delta || user_delta < 0 && user_delta < delta) && AngularDistance(user_delta, delta) > 3) {
		pm->ps->viewangles[YAW] += delta;
		fvec3(pm->ps->viewangles).toforward().assign_to(pml->forward);
		fvec3(pm->ps->viewangles).toright().assign_to(pml->right);
		fvec3(pm->ps->viewangles).toup().assign_to(pml->up);
		Com_Printf(CON_CHANNEL_OBITUARY, "^1overstrafe by (%.2f, %.2f)\n", delta, user_delta );
	}
	old_yaw = pm->ps->viewangles[YAW];
}