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

	T_AutoPara(pm, pml);

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
void T::Movement::T_AutoPara(pmove_t* pm, pml_t* pml)
{
	static decltype(auto) auto_para = find_evar<bool>("Auto Para");
	static char _rightmove = 0;

	if (auto_para->get() == false || pm->ps->speed != 190) {
		_rightmove = 0;
		return;
	}
	const float speed = fvec2(pm->ps->velocity[0], pm->ps->velocity[1]).mag();

	if ((speed <= 285 || speed >= 625) || pm->cmd.forwardmove == NULL || pm->ps->groundEntityNum != 1023) {
		_rightmove = 0;
		return;
	}

	if (!_rightmove)
		_rightmove = pm->cmd.rightmove;

	if(_rightmove)
		pm->cmd.rightmove = _rightmove;

	static std::vector<fps_zone> zones = GetFPSZones(pm->ps->speed);

	if (zones.empty())
		return FatalError("T_AutoPara(): no fps zones");

	const int32_t FPS = T_GetIdealFPS(pm);


	//pm->cmd.rightmove = 0;

	auto results = GetDistanceToFPSZone(pm, 333);

	float distance2inneredge = _rightmove == 127 ? results.end : results.begin;

	if (distance2inneredge < 0.1f || distance2inneredge >= results.length + 10) {
		_rightmove *= -1;
	}
	pm->oldcmd.rightmove = _rightmove;
	pm->cmd.rightmove = _rightmove;

	Dvar_FindMalleableVar("com_maxfps")->current.integer = 333;

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

	com_maxfps->current.integer = T_GetIdealFPS(pm);


}

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
std::vector<fps_zone> T::Movement::GetFPSZones(int g_speed)
{
	std::vector<int> fps;
	std::vector<fps_zone> zone;

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

	fps_zone copy;

	zone.resize(fps.size());
	zone[0].start = (std::round(g_speed / (1000 / fps[0])));
	zone[0].end = -zone[0].start;
	zone[0].FPS = fps[0];

	if (g_speed == 190) {
		zone[0].start -= 1;
		zone[0].end = -zone[0].start;
	}

	zone[0].start = int(zone[0].start);
	zone[0].end = int(zone[0].end);

	copy = zone[0];
	swapForAxis(copy);

	zone[0].length = (zone[0].start > 45 ? 90.f - zone[0].start : zone[0].start) * 2;


	for (int i = 1; i < fps.size(); i++) {

		zone[i].start = zone[i - 1].end;
		zone[i].end = zone[i].start * float(1000 / fps[i - 1]) / float(1000 / fps[i]);

		zone[i].start = int(zone[i].start);
		zone[i].end = int(zone[i].end);
		zone[i].FPS = fps[i];
		zone[i].length = (zone[i].start > 45 ? 90.f - zone[i].start : zone[i].start) * 2;


	}

	return zone;
}
int32_t T::Movement::T_GetIdealFPS(pmove_t* pm)
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
	static std::vector<fps_zone> zones = GetFPSZones(pm->ps->speed);
	fps_zone copy;
	if (g_speed != pm->ps->speed) {
		zones = GetFPSZones(pm->ps->speed);
		g_speed = pm->ps->speed;
	}

	copy = zones[0];


	if (pm->cmd.rightmove == 127) {
		copy.start *= -1;
		copy.end *= -1;

	}

	if (!in_range(pm->ps->viewangles[YAW], copy.start, copy.end, 0))
		return 333;

	swapForAxis(copy);

	if (in_range(pm->ps->viewangles[YAW], copy.start, copy.end, 0))
		return 333;


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
zone_distance T::Movement::GetDistanceToFPSZone(pmove_t* pm, int wishFPS)
{
	static int g_speed = 0;
	static std::vector<fps_zone> zones = GetFPSZones(pm->ps->speed);
	if (g_speed != pm->ps->speed) {
		zones = GetFPSZones(pm->ps->speed);
		g_speed = pm->ps->speed;
	}

	fps_zone* zone = 0;

	std::vector<fps_zone>::iterator it = (std::find_if(zones.begin(), zones.end(), [wishFPS](const fps_zone& z) {
		return (wishFPS == z.FPS); }));

	if(it == zones.end())
		FatalError("GetFPSZoneDistances didn't find matching fps");

	zone = &*it;

	char rightmove = pm->cmd.rightmove;

	if (rightmove >= 0)
		rightmove *= -1;

	const float yaw = AngleNormalize90(pm->ps->viewangles[YAW] + RAD2DEG(atan2(-(int)rightmove, (int)pm->cmd.forwardmove))); //yaw + accel angle
	const float real_yaw = AngleNormalize90(pm->ps->viewangles[YAW]);;
	zone_distance results;

	results.begin = AngleNormalize90((AngleDelta(yaw, zone->end)));
	results.end = AngleNormalize90((AngleDelta(yaw, zone->start)));

	if (real_yaw >= 0) {
		results.begin *= -1;
	}
	else {
		results.begin = 90.f - results.begin;
		results.end += 90.f;

		if (results.begin >= 90)
			results.begin -= 180;
		if (results.end >= 90)
			results.end -= 180;
	}

	if (results.begin < 0)
		results.begin += 90;
	if (results.end < 0)
		results.end += 90;

	results.length = zone->length;
		
	return results;

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