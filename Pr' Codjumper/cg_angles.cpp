#include "pch.hpp"

float AngleNormalizePI(float angle)
{
	angle = fmodf(angle + (float)M_PI, 2 * (float)M_PI);
	return angle < 0 ? angle + (float)M_PI : angle - (float)M_PI;
}
float AngleNormalize360(float angle) {
	return (360.0f / 65536) * ((int)(angle * (65536 / 360.0f)) & 65535);
}
float AngleNormalize180(float angle) {
	angle = AngleNormalize360(angle);
	if (angle > 180.0) {
		angle -= 360.0;
	}
	return angle;
}

float AngleDelta(float angle1, float angle2) {
	return AngleNormalize180(angle1 - angle2);
}

std::optional<float> CG_GetOptYawDelta(pmove_t* pm, pml_t* pml)
{
	char forwardmove = pm->cmd.forwardmove;
	char rightmove = pm->cmd.rightmove;

	float speed = fvec2(pm->ps->velocity).mag();

	if (speed < 1 || forwardmove != 127 && rightmove != 0)
		return std::nullopt;


	float g_speed = pm->ps->speed;
	int FPS = 1000 / pml->msec;

	const float accel = g_speed / FPS;

	WeaponDef* weapon = BG_WeaponNames[pm->ps->weapon];

	if (pm->ps->groundEntityNum == 1022) {
		g_speed = pm->ps->speed / (weapon->moveSpeedScale * (pml->groundTrace.normal[2])) / 1.02150f;


		if ((pm->cmd.buttons & 8194) != 0){
			g_speed = (pm->ps->speed / (weapon->moveSpeedScale * (pml->groundTrace.normal[2])) * Dvar_FindMalleableVar("player_sprintSpeedScale")->current.value) / 1.26106f;

			if (GetAsyncKeyState(VK_MBUTTON) & 1)
				Com_Printf(CON_CHANNEL_OBITUARY, "^2%i\n", (int)g_speed);
		}

	}

	const float velocitydirection = atan2(pm->ps->velocity[1], pm->ps->velocity[0]) * 180.f / PI;
	const float accelerationAng = atan2(-rightmove, forwardmove) * 180.f / PI;
	const float diff = acos((g_speed - accel) / speed) * 180.f / PI;

	float& yaw = pm->ps->viewangles[YAW];

	if (std::isnan(diff))
		return std::nullopt;

	if (rightmove > 0) {
		return AngleDelta(yaw + accelerationAng, (velocitydirection - diff));
	}
	else if (rightmove < 0) {
		return AngleDelta(yaw + accelerationAng, (velocitydirection + diff));
	}

	return std::nullopt;
}