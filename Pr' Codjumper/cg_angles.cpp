#include "pch.hpp"
void CG_SetYaw(const float ang)
{
	float ref = clients->cgameViewangles[YAW];
	ref = fmodf(ref, 360);
	ref -= ref * 2 - ang;
	clients->viewangles[1] += ref;

}
void CG_SetPitch(const float ang)
{
	float ref = clients->cgameViewangles[PITCH];
	ref = fmodf(ref, 360);
	ref -= ref * 2 - ang;
	clients->viewangles[0] += ref;

}
void CG_SetRoll(const float ang)
{
	float ref = clients->cgameViewangles[ROLL];
	ref = fmodf(ref, 360);
	ref -= ref * 2 - ang;
	clients->viewangles[2] += ref;

}
void CG_SetPlayerAngles(const fvec3& target)
{
	CG_SetPitch(target.x);
	CG_SetYaw(target.y);
	CG_SetRoll(target.z);
}
void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up)
{
	float angle;
	static float sr, sp, sy, cr, cp, cy;
	// static to help MS compiler fp bugs

	angle = angles[YAW] * (M_PI * 2 / 360);
	sy = sin(angle);
	cy = cos(angle);

	angle = angles[PITCH] * (M_PI * 2 / 360);
	sp = sin(angle);
	cp = cos(angle);

	angle = angles[ROLL] * (M_PI * 2 / 360);
	sr = sin(angle);
	cr = cos(angle);

	if (forward) {
		forward[0] = cp * cy;
		forward[1] = cp * sy;
		forward[2] = -sp;
	}
	if (right) {
		right[0] = (-1 * sr * sp * cy + -1 * cr * -sy);
		right[1] = (-1 * sr * sp * sy + -1 * cr * cy);
		right[2] = -1 * sr * cp;
	}
	if (up) {
		up[0] = (cr * sp * cy + -sr * -sy);
		up[1] = (cr * sp * sy + -sr * cy);
		up[2] = cr * cp;
	}
}
float ProjectionX(float angle, float fov)
{
	float SCREEN_WIDTH = cgs->refdef.width;
	float const half_fov_x = DEG2RAD(fov) / 2;
	if (angle >= half_fov_x)
	{
		return 0;
	}
	if (angle <= -half_fov_x)
	{
		return SCREEN_WIDTH;
	}

	return SCREEN_WIDTH / 2 * (1 - angle / half_fov_x);

}
range_t AnglesToRange(float start, float end, float yaw, float fov)
{
	start = DEG2RAD(start);
	end = DEG2RAD(end);
	yaw = DEG2RAD(yaw);

	if (fabsf(end - start) > 2 * (float)M_PI)
	{
		return std::move(range_t{ 0, float(cgs->refdef.width), false });
	}

	bool split = end > start;
	start = AngleNormalizePI(start - yaw);
	end = AngleNormalizePI(end - yaw);


	if (end > start)
	{
		split = !split;
		std::swap(start, end);
	}

	return std::move(range_t{ ProjectionX(start, fov), ProjectionX(end, fov), split });

}

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
float AngleNormalize90(float angle)
{
	return fmodf(angle + 180 + 90, 180) - 90;

}
float AngleDelta(float angle1, float angle2) {
	return AngleNormalize180(angle1 - angle2);
}

std::optional<float> CG_GetOptYawDelta(pmove_t* pm, pml_t* pml)
{
	char forwardmove = pm->cmd.forwardmove;
	char rightmove = pm->cmd.rightmove;

	float speed = fvec2(pm->ps->velocity).mag();

	if (speed < 1 || forwardmove != 127 || rightmove == 0)
		return std::nullopt;


	float g_speed = pm->ps->speed;
	float FPS = 1000.f / pml->msec;

	float accel = FPS / g_speed * pow(333 / FPS, 2);

	if (accel < 1)
		accel = g_speed / FPS;

	WeaponDef* weapon = BG_WeaponNames[pm->ps->weapon];

	if (pm->ps->groundEntityNum == 1022) {
		g_speed = pm->ps->speed / (weapon->moveSpeedScale * (pml->groundTrace.normal[2])) / 1.02150f;


		if ((pm->cmd.buttons & 8194) != 0){
			g_speed = (pm->ps->speed / (weapon->moveSpeedScale * (pml->groundTrace.normal[2])) * Dvar_FindMalleableVar("player_sprintSpeedScale")->current.value) / 1.26106f;
		}

	}

	const float velocitydirection = atan2(pm->ps->velocity[1], pm->ps->velocity[0]) * 180.f / PI;
	const float accelerationAng = atan2(-rightmove, forwardmove) * 180.f / PI;
	const float diff = acos((g_speed - accel) / speed) * 180.f / PI;

	float yaw = pm->ps->viewangles[YAW];

	if (std::isnan(diff))
		return std::nullopt;

	if (rightmove > 0) {
		return -AngleDelta(yaw + accelerationAng, (velocitydirection - diff));
	}
	else if (rightmove < 0) {
		return -AngleDelta(yaw + accelerationAng, (velocitydirection + diff));
	}

	return std::nullopt;
}
float AngularDistance(float value1, float value2) {
	float diff = fmod(value2 - value1 + 180, 360) - 180;
	if (diff < -180) {
		diff += 360;
	}
	return std::abs(diff);
}