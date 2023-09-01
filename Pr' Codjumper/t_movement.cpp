#include "pch.hpp"

bool T::Movement::StrafingRight(pmove_t* pm)
{
	return atan2(-pm->cmd.rightmove, pm->cmd.forwardmove) < 0;
}
bool T::Movement::StrafingLeft(pmove_t* pm)
{
	return atan2(-pm->cmd.rightmove, pm->cmd.forwardmove) > 0;
}

void T::Movement::T_Strafebot(pmove_t* pm, pml_t* pml)
{
	static decltype(auto) strafebot = find_evar<bool>("Strafebot");

	if (strafebot->get() == false)
		return;

	std::optional<float> yaw;


	if ((yaw = CG_GetOptYawDelta(pm, pml)) == std::nullopt) {
		PM_DisallowMouse(false);
		return;
	}

	PM_DisallowMouse(true);

	//pm->cmd.angles[YAW] += ANGLE2SHORT(yaw.value());
	//pm->ps->viewangles[YAW] += yaw.value();

	float delta = yaw.value();

	if (AngularDistance(0.f, delta) >= 90) {
		Com_Printf(CON_CHANNEL_OBITUARY, "^2%.6f\n", delta);
	}

	pm->ps->viewangles[YAW] += delta;
	pm->cmd.angles[YAW] += ANGLE2SHORT(delta);
	pm->ps->delta_angles[YAW] -= delta;

	//fvec3(pm->ps->viewangles).toforward().assign_to(pml->forward);
	//fvec3(pm->ps->viewangles).toright().assign_to(pml->right);
	//fvec3(pm->ps->viewangles).toup().assign_to(pml->up);

	
}