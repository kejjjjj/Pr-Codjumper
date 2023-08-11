#pragma once

#include "pch.hpp"

struct PlayerMouse
{
	static PlayerMouse& get() { static PlayerMouse m; return m; }
	
	fvec2 delta;
};

void CL_GetMouseMovement(clientActive_t* client, float* my, float* mx);