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
	if (AngularDistance(zone.end, 0) > 45 && AngularDistance(zone.start, 0) > 45) {
		zone.start = -AngularDistance(zone.start, 90);
		zone.end = AngularDistance(zone.end, -90);
		return;

	}
	else if (AngularDistance(zone.end, 0) > 45) {
		zone.start = AngularDistance(zone.start, -90);
		zone.end = AngularDistance(zone.end, 90);
		return;
	}



	zone.start = AngularDistance(zone.start, -90);
	zone.end = AngularDistance(zone.end, -90);

	return;
}

int32_t T::Movement::T_GetIdealFPS(pmove_t* pm, pml_t* pml)
{
	float screen_center = float(cgs->refdef.width / 2);
	const auto in_range = [&screen_center, &pm](float yaw, float min, float max, bool info) -> bool {
		
		float aa = RAD2DEG(atan2(-(int)pm->cmd.rightmove, (int)pm->cmd.forwardmove));
		yaw = AngleNormalize90(yaw + aa);
		const float fov = Dvar_FindMalleableVar("cg_fov")->current.value * Dvar_FindMalleableVar("cg_fovscale")->current.value;

		range_t results = AnglesToRange(min, max, yaw, fov);

		return results.x1 <= screen_center && results.x2 >= screen_center;
	};

	int fps_list[] = { 333, 250, 200, 125, 111 };
	fps_zone zones[4];

	zones[F333].start = AngularDistance(std::round(pm->ps->speed / (1000 / fps_list[F333])), 0.f);
	zones[F333].end = -zones[0].start;
	zones[F333].FPS = 333;

	zones[F250].start = zones[F333].end;
	zones[F250].end = zones[F333].start * float(1000 / fps_list[F333]) / float(1000 / fps_list[F250]);
	zones[F250].FPS = 250;

	if (zones[F250].end > 45)
		zones[F250].end = -AngularDistance(zones[F250].end, 90.f);

	zones[F200].start = zones[F250].end;
	zones[F200].end = zones[F200].start * float(1000 / fps_list[F250]) / float(1000 / fps_list[F200]);
	zones[F200].FPS = 200;

	zones[F125].start = zones[F200].end;
	zones[F125].end = zones[F125].start * float(1000 / fps_list[F200]) / float(1000 / fps_list[F125]);
	zones[F125].FPS = 125;

	bool pressed = GetAsyncKeyState(VK_NUMPAD8) & 1;

	if (pressed) {
		for (int zone = 0; zone < 4; zone++) {

			std::cout << zones[zone].FPS << " = (" << zones[zone].start << ", " << zones[zone].end << ") -> ";
			auto copy = zones[zone];

			swapForAxis(copy);

			std::cout << "(" << copy.start << ", " << copy.end << ")\n";

		}
	}


	for (int zone = 1; zone < 4; zone++) {

		zones[zone].start = int(zones[zone].start);
		zones[zone].end = int(zones[zone].end);

		if (pm->cmd.rightmove == 127) {
			zones[zone].start *= -1;
			zones[zone].end *= -1;

		}

		if (in_range(pm->ps->viewangles[YAW], zones[zone].start, zones[zone].end, pressed))
			return zones[zone].FPS;
		
		swapForAxis(zones[zone]);

		if (in_range(pm->ps->viewangles[YAW], zones[zone].start, zones[zone].end, pressed))
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