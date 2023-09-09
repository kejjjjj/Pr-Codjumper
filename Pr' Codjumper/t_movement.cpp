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

	pm->ps->viewangles[YAW] += delta;
	pm->cmd.angles[YAW] += ANGLE2SHORT(delta);
	pm->ps->delta_angles[YAW] -= delta;

	//fvec3(pm->ps->viewangles).toforward().assign_to(pml->forward);
	//fvec3(pm->ps->viewangles).toright().assign_to(pml->right);
	//fvec3(pm->ps->viewangles).toup().assign_to(pml->up);

	
}
void T::Movement::T_AutoFPS(pmove_t* pm, pml_t* pml)
{

	auto var = find_evar<bool>("AutoFPS");

	if (!var->get())
		return;
	
	playerState_s* ps = pm->ps;

	static dvar_s* com_maxfps = Dvar_FindMalleableVar("com_maxfps");

	if (ps->groundEntityNum == 1022) {
		com_maxfps->current.integer = 125;
		return;
	}

	com_maxfps->current.integer = T_GetIdealFPS(pm, pml);


}

enum fps_enum
{
	F333,
	F250,
	F200,
	F125
};
struct fps_zone
{
	float start;
	float end;
	int FPS;
};
void swapForAxis(fps_zone& zone)
{
	if (AngularDistance(zone.end, 0) > 50 && AngularDistance(zone.start, 0) > 45) {
		zone.start = -AngularDistance(zone.start, 90);
		zone.end = AngularDistance(zone.end, -90);
		return;

	}
	else if (AngularDistance(zone.end, 0) > 45) {
		zone.start = AngularDistance(zone.start, -90);
		zone.end = AngularDistance(zone.end, -90);
		return;
	}



	zone.start = AngularDistance(zone.start, -90);
	zone.end = AngularDistance(zone.end, -90);

	return;
}
void get_zones(int g_speed, std::vector<fps_zone>& zone)
{
	std::vector<int> fps;
	zone.clear();

	//note: g_speed will mess these hardcoded values up!
	fps.push_back(333);
	fps.push_back(250);
	fps.push_back(200);
	fps.push_back(125);
	//fps.push_back(111);
	//fps.push_back(100);


	//use this if you need g_speed

	//for (int i = 2; i < 5; i++) {

	//	float v = std::round(float(g_speed) / (1000.f / i));
	//	if (v == 0.f)
	//		continue;

	//	if (fps.empty() == false)
	//		if (fps.back() == 1000 / i)
	//			continue;

	//	fps.push_back(1000 / i);



	//}

	zone.resize(fps.size());
	zone[0].start = (std::round(g_speed / (1000 / fps[0])));
	zone[0].end = -zone[0].start;
	zone[0].FPS = fps[0];

	zone[0].start = int(zone[0].start);
	zone[0].end = int(zone[0].end);

	for (int i = 1; i < fps.size(); i++) {

		zone[i].start = zone[i - 1].end;
		zone[i].end = zone[i].start * float(1000 / fps[i - 1]) / float(1000 / fps[i]);

		zone[i].start = int(zone[i].start);
		zone[i].end = int(zone[i].end);
		zone[i].FPS = fps[i];

	}

	return;
}
int32_t T::Movement::T_GetIdealFPS(pmove_t* pm, pml_t* pml)
{
	const auto in_range = [&pm](float yaw, float min, float max, bool info) -> bool {
		const float screen_center = float(cgs->refdef.width / 2);

		const float aa = RAD2DEG(atan2(-(int)pm->cmd.rightmove, (int)pm->cmd.forwardmove));
		yaw = AngleNormalize90(yaw + aa);
		const float fov = Dvar_FindMalleableVar("cg_fov")->current.value * Dvar_FindMalleableVar("cg_fovscale")->current.value;

		range_t results = AnglesToRange(min, max, yaw, fov);

		return results.x1 <= screen_center && results.x2 >= screen_center;
	};
	static int g_speed = 0;
	static std::vector<fps_zone> zones;
	fps_zone copy;
	if (g_speed != pm->ps->speed) {
		get_zones(pm->ps->speed, zones);
		g_speed = pm->ps->speed;
	}

	for (int zone = 1; zone < zones.size(); zone++) {

		copy = zones[zone];

		if (pm->cmd.rightmove == 127) {
			copy.start *= -1;
			copy.end *= -1;

		}

		if (in_range(pm->ps->viewangles[YAW], copy.start, copy.end, 0))
			return zones[zone].FPS;
		
		swapForAxis(copy);

		if (in_range(pm->ps->viewangles[YAW], copy.start, copy.end, 0))
			return zones[zone].FPS;

	}

	

	return 333;

}

//void T::Movement::T_QuickTest()
//{
//	std::vector<int> best_fps(90);
//	std::list<int> fps_list = { 125, 200, 250, 333 };
//	std::list<float> accel_list;
//	float best_accel = 0;
//
//	PM_MovementSimulation sim(pm_glob, pml_glob, pm_controller_t
//		{
//			.forwardmove = 127,
//			.rightmove = -127,
//			.buttons = 0,
//			.FPS = 125,
//			.numLoops = 2,
//			.velocity = std::make_optional<fvec3>({285,0,0}),
//			.origin = std::make_optional<fvec3>({0,0,100000}),
//			.viewangles = std::make_optional<fvec3>({0,0,0}),
//			.turnrate = 1,
//			.strafebot = true
//		});
//
//	for (int i = 0; i < 90; i++) {
//		best_accel = 0;
//		for (auto& fps : fps_list) {
//
//
//
//			sim.StartSimulation();
//
//			pmove_t* pm = sim.GetPM();
//			float length = pm->ps->velocity[0] * pm->ps->velocity[0] + pm->ps->velocity[1] * pm->ps->velocity[1];
//
//			if (best_accel < length) {
//				best_fps[i] = fps;
//				best_accel = length;
//				std::cout << fps << " | " << sqrt(length) << '\n';
//			}
//		}
//	}
//	for (int i = 0; i < best_fps.size(); i++) {
//		std::cout << i << " = " << best_fps[i] << '\n';
//	}
//}