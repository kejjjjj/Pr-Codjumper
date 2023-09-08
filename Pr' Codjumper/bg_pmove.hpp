#pragma once

#include "pch.hpp"

bool PM_DisallowMouse(const std::optional<bool>& state=std::nullopt);

void PM_WalkMove(pmove_t* pm, pml_t* pml);
void PM_AirMove(pmove_t* pm, pml_t* pml);
void PM_UpdateViewAngles(playerState_s* ps, float msec, usercmd_s* cmd, char handler);
void Sys_SnapVector(float* v);