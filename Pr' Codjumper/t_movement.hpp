#pragma once

#include "pch.hpp"

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
	float length;
};

struct zone_distance
{
	float begin;
	float end;
	float length;
};

namespace T::Movement
{
	bool StrafingRight(pmove_t* pm);
	bool StrafingLeft(pmove_t* pm);

	void T_Strafebot(pmove_t* pm, pml_t* pml);
	void T_AutoPara(pmove_t* pm, pml_t* pml);

	std::vector<fps_zone> GetFPSZones(int g_speed);

	int32_t T_GetIdealFPS(pmove_t* pm);
	
	zone_distance GetDistanceToFPSZone(pmove_t* pm, int wishFPS);

	void T_AutoFPS(pmove_t* pm, pml_t* pml);

	inline bool T_Ready() noexcept { return pm_glob && pml_glob && pm_glob->ps && !NOT_SERVER; }

}